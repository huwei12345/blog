#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <qqueue.h>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      tcpClient(NULL),
      user(NULL),
      my_user(NULL),
      article_count(0),
      comment_count(0),
      group_now(0),
      ui(new Ui::MainWindow),
      group(NULL),
      article(NULL),
      now_comment(NULL)
{
    ui->setupUi(this);
     connect(ui->treeWidget,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),this,SLOT(Double_click(QTreeWidgetItem*,int)));
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::setfather(QWidget *father)
{
    this->father=father;
}

void MainWindow::update_group(Group *group,int count)
{
    ui->treeWidget->clear();
    //设置水平头
    ui->treeWidget->setHeaderLabels(QStringList()<<"组/文章id"<<"文章标题");
    QTreeWidgetItem *MyItem=new QTreeWidgetItem(QStringList()<<QString("user_id:")+QString::number(user->user_id));
    ui->treeWidget->addTopLevelItem(MyItem);
    //加载根节点
    if(group!=NULL)
    {
        QQueue<int> que;
        QQueue<QTreeWidgetItem*> widget_que;
        que.enqueue(0);
        widget_que.enqueue(MyItem);
        QTreeWidgetItem* temp=NULL;
        QTreeWidgetItem* p=NULL;
        //广度优先搜索，构建文章组树
        while(!que.empty())
        {
            int now=que.dequeue();
            temp=widget_que.dequeue();
            for(int i=0;i<count;i++)
            {
                if(group[i].father_group_id==now)
                {
                    p=new QTreeWidgetItem(QStringList()<<QString("id:")+QString::number(group[i].group_id)<<group[i].group_name);
                    temp->addChild(p);
                    widget_que.enqueue(p);
                    que.enqueue(group[i].group_id);
                }
            }
        }
    }
    qDebug()<<"group build already";
    if(group!=NULL)
        delete[] group;
}

void MainWindow::update_article_list(Article *article, int count)
{
    for(int i=0;i<count;i++)//对每一个article遍历
    {
        QTreeWidget* unitTree=ui->treeWidget;
        QTreeWidgetItemIterator iterator(unitTree);
        while (*iterator){
            if ((*iterator)->text(0)==QString("id:")+QString::number(article[i].group_id)) {
                //找到目标 item ，做相关操作
                QTreeWidgetItem *item = (*iterator);
                QTreeWidgetItem *art=new QTreeWidgetItem(QStringList()<<QString::number(article[i].art_id)<<article[i].title);
                item->addChild(art);
                break;
            }
            ++iterator;
        }
        if(article[i].group_id==0)
        {
            QTreeWidgetItem *art=new QTreeWidgetItem(QStringList()<<QString::number(article[i].art_id)<<article[i].title);
            ui->treeWidget->topLevelItem(0)->addChild(art);//不应该写死
        }
    }
    if(article!=NULL)
        delete[] article;
}

void MainWindow::update_article(Article *article)
{
    ui->plainTextEdit->clear();
    if(article!=NULL)
    {

        QByteArray abc = QByteArray::fromBase64(article->text,QByteArray::Base64UrlEncoding | QByteArray::OmitTrailingEquals);
        ui->plainTextEdit->setPlainText(abc);

        //ui->plainTextEdit->setPlainText(article->text);
        ui->lineEdit_2->setText(article->title);
        ui->lineEdit->setText(QString::number(article->upvote_num));
        if(this->article!=NULL)
        {
            delete this->article;
            this->article=NULL;
        }
        this->article=article;
        ui->comboBox->setCurrentIndex(article->type-1);
    }
}

void MainWindow::update_comment(Comment *comment, int count)
{
    ui->listWidget->clear();
    for(int i=0;i<count;i++)
    {
        if(comment[i].text!=NULL)
            ui->listWidget->addItem(QString::number(comment[i].comment_id)+QString("    ")+comment[i].text);
    }
    if(comment!=NULL)
        delete[] comment;
}

void MainWindow::insert_article(Article *article, int count)
{
    Q_UNUSED(count);
    //在左侧栏加入文章标题、id
    count=1;
    QTreeWidgetItem* unitTree=ui->treeWidget->currentItem();
    QTreeWidgetItem *art=new QTreeWidgetItem(QStringList()<<QString::number(article->art_id)<<ui->lineEdit_2->text());
    unitTree->addChild(art);
    if(article!=NULL)
        delete[] article;
}

void MainWindow::on_upload_triggered()
{

}

void MainWindow::on_return_btn_triggered()
{
    //返回按钮
    father->show();
    this->close();
}

void MainWindow::Double_click(QTreeWidgetItem *item, int)
{
    //双击树中节点
    Article* article=new Article;
    article->art_id=item->text(0).toInt();
    if(this->article!=NULL)
    {
        delete this->article;
        this->article=NULL;
    }
    this->article=article;
    qDebug()<<"double click art_id:"<<article->art_id;

    if(!item->text(0).startsWith("id:")&&!item->text(0).startsWith("user"))
    {
        init_other_article();

        char *buf=new char[500];
        memset(buf,'\0',500);
        short type=query_article_t;//登录，返回User
        char* str=NULL;
        //如果有对象数据要传输
        str=struct2json(article,ARTICLE,1);
        int length=package_request(buf,str,type);
        //发送
        tcpClient->write(buf,length); //qt5去除了.toAscii()
        tcpClient->waitForBytesWritten();
        tcpClient->flush();


        //对文章评论，包到达先后顺序无所谓
        memset(buf,'\0',500);
        type=query_comment_t;//登录，返回User
        str=NULL;
        Comment* comment=new Comment;
        comment->art_id=article->art_id;
        //如果有对象数据要传输
        str=struct2json(comment,COMMENT,1);
        length=package_request(buf,str,type);
        qDebug()<<"长度:"<<length<<"json:"<<buf+12;
        //发送
        tcpClient->write(buf,length); //qt5去除了.toAscii()
        delete comment;
    }
}

void MainWindow::init_other_article()
{
    if(user->user_id!=my_user->user_id)
    {
        char* str=NULL;
        char* buf=new char[500];
        int length=0;
        int type=0;
        //对收藏状态
        Collect* col=new Collect;
        col->user_id=my_user->user_id;
        col->collect_art_id=article->art_id;

        str=struct2json(col,COLLECT,1);
        memset(buf,'\0',500);
        type=query_collect_simple_t;//登录，返回User
        length=package_request(buf,str,type);
        delete col;
        //发送
        tcpClient->write(buf,length); //qt5去除了.toAscii()
        tcpClient->waitForBytesWritten();
        tcpClient->flush();
        delete[] buf;
    }
}
void MainWindow::on_pushButton_clicked()
{
    //字体
    QFont iniFont=ui->plainTextEdit->font();
    bool isok;
    QFont qf = QFontDialog::getFont(&isok,iniFont);
    if(isok)
    {
        ui->plainTextEdit->setFont(qf);
    }
}

void MainWindow::on_pushButton_2_clicked()
{
    //颜色
    QPalette pal=ui->plainTextEdit->palette();
    QColor iniColor=pal.color(QPalette::Text);//现有文字颜色
    QColor cl = QColorDialog::getColor(iniColor,this,"选择颜色");
    if(cl.isValid())
    {
        pal.setColor(QPalette::Text,cl);//颜色的载体
        ui->plainTextEdit->setPalette(pal);
    }
}

void MainWindow::on_pushButton_4_clicked()
{
    //全选
    ui->plainTextEdit->setFocus();
    QTimer::singleShot(0, ui->plainTextEdit, &QPlainTextEdit::selectAll);
}

void MainWindow::on_pushButton_5_clicked()
{
    //复制
    ui->plainTextEdit->copy();
}

void MainWindow::on_pushButton_6_clicked()
{
    //黏贴
    ui->plainTextEdit->paste();
}

void MainWindow::on_pushButton_3_clicked()
{
    //剪切
    ui->plainTextEdit->cut();
}

static bool bPause1=true;
static bool bPause3=true;
void MainWindow::on_pushButton_8_clicked()
{
    //关注
    if(bPause1==true)
    {
        ui->pushButton_8->setText("取消关注");
        bPause1=false;
        //关注

        User_Relation* rel=new User_Relation;
        rel->user_id=my_user->user_id;
        rel->rel_user_id=user->user_id;

        char* str=struct2json(rel,USER_REL,1);
        char *buf=new char[100];
        memset(buf,'\0',100);
        short type=insert_user_rel_t;//登录，返回User
        int length=package_request(buf,str,type);
        delete rel;
        //发送
        tcpClient->write(buf,length); //qt5去除了.toAscii()
        tcpClient->waitForBytesWritten();
        tcpClient->flush();
        delete[] buf;
    }
    else
    {
        ui->pushButton_8->setText("关注");
        bPause1=true;
        //取关

        User_Relation* rel=new User_Relation;
        rel->user_id=my_user->user_id;
        rel->rel_user_id=user->user_id;

        char* str=struct2json(rel,USER_REL,1);
        char *buf=new char[100];
        memset(buf,'\0',100);
        short type=delete_user_rel_t;//登录，返回User
        int length=package_request(buf,str,type);
        delete rel;
        //发送
        tcpClient->write(buf,length); //qt5去除了.toAscii()
        tcpClient->waitForBytesWritten();
        tcpClient->flush();
        delete[] buf;
    }
}
void MainWindow::on_pushButton_7_clicked()
{
    //点赞，只增加数目
    Article* article=new Article;
    article->user_id=user->user_id;
    article->art_id=this->article->art_id;

    char* str=struct2json(article,ARTICLE,1);
    char *buf=new char[100];
    memset(buf,'\0',100);
    short type=add_art_upvote_t;//登录，返回User
    int num=ui->lineEdit->text().toInt()+1;
    ui->lineEdit->setText(QString::number(num));
    int length=package_request(buf,str,type);
    delete article;
    //发送
    tcpClient->write(buf,length); //qt5去除了.toAscii()
    tcpClient->waitForBytesWritten();
    tcpClient->flush();
    delete[] buf;
}


void MainWindow::on_pushButton_10_clicked()
{
    //收藏
    if(bPause3==true)
    {
        ui->pushButton_10->setText("取消收藏");
        bPause3=false;
        Collect* collect=new Collect;
        collect->user_id=my_user->user_id;
        collect->collect_art_id=article->art_id;

        char* str=struct2json(collect,COLLECT,1);
        char *buf=new char[100];
        memset(buf,'\0',100);
        short type=insert_collect_t;//登录，返回User
        int length=package_request(buf,str,type);
        delete collect;
        //发送
        tcpClient->write(buf,length); //qt5去除了.toAscii()
        tcpClient->waitForBytesWritten();
        tcpClient->flush();
        delete[] buf;
    }
    else
    {
        ui->pushButton_10->setText("收藏");
        bPause3=true;

        Collect* collect=new Collect;
        collect->user_id=my_user->user_id;
        collect->collect_art_id=article->art_id;

        char* str=struct2json(collect,COLLECT,1);
        char *buf=new char[100];
        memset(buf,'\0',100);
        short type=delete_collect_t;//登录，返回User
        int length=package_request(buf,str,type);
        delete collect;
        //发送
        tcpClient->write(buf,length); //qt5去除了.toAscii()
        tcpClient->waitForBytesWritten();
        tcpClient->flush();
        delete[] buf;
    }
}

void MainWindow::init_other_blog()
{
    //7 8 9 点赞、关注、收藏
    QList<QAction*> action=ui->toolBar->actions();
    if(my_user->user_id!=user->user_id)
    {
        //访问他人博客
        ui->pushButton_7->setEnabled(true);
        ui->pushButton_8->setEnabled(true);
        ui->pushButton_9->setEnabled(true);
        ui->pushButton_10->setEnabled(true);

        action[1]->setDisabled(true);
        action[3]->setDisabled(true);
        action[4]->setDisabled(true);

        ui->toolButton->setDisabled(true);
        ui->toolButton_2->setDisabled(true);
        ui->toolButton_3->setDisabled(true);
        ui->toolButton_4->setDisabled(true);
        //获取关注状态
        User_Relation* rel=new User_Relation;
        rel->user_id=my_user->user_id;
        rel->rel_user_id=user->user_id;

        char* str=struct2json(rel,USER_REL,1);
        char *buf=new char[100];
        memset(buf,'\0',100);
        short type=query_user_rel_simple_t;//登录，返回User
        int length=package_request(buf,str,type);
        delete rel;
        //发送
        tcpClient->write(buf,length); //qt5去除了.toAscii()
        tcpClient->flush();
        delete[] buf;
    }
    else
    {

        ui->pushButton_7->setDisabled(true);
        ui->pushButton_8->setDisabled(true);
        ui->pushButton_10->setDisabled(true);
        ui->pushButton_9->setDisabled(true);
        action[1]->setEnabled(true);
        action[3]->setEnabled(true);
        action[4]->setEnabled(true);

        ui->toolButton->setEnabled(true);
        ui->toolButton_2->setEnabled(true);
        ui->toolButton_3->setEnabled(true);
        ui->toolButton_4->setEnabled(true);
    }
}

void MainWindow::set_rel_on()
{
    ui->pushButton_8->setText("取消关注");
    qDebug()<<"当前是关注的";
    bPause1=false;
}

void MainWindow::set_col_on()
{
    ui->pushButton_10->setText("取消收藏");
    bPause3=false;
}

void MainWindow::set_rel_down()
{
    ui->pushButton_8->setText("关注");
    bPause1=true;
    qDebug()<<"当前是没关注";
}

void MainWindow::set_col_down()
{
    ui->pushButton_10->setText("收藏");
    bPause3=true;
}

void MainWindow::on_action_7_triggered()
{
    //新建
    Article* article=new Article;
    QTreeWidgetItem* item=ui->treeWidget->currentItem();
    QString str_group=item->text(0);
    if(str_group.startsWith("id:"))
    {
        char* s=str_group.toLatin1().data();
        s=s+3;
        int num=0;
        for(unsigned int i=0;i<strlen(s);i++)
            qDebug()<<s[i]-'0';
        int i=0;
        while(s[i]!='\0'&&s[i]>='0'&&s[i]<='9')
        {
            num=num*10+(s[i]-'0');
            i++;
        }
        article->group_id=num;
    }
    else if(str_group.startsWith("user_id:"))
    {
        article->group_id=0;
    }
    else
    {
        QMessageBox::information(this,"新建文章无效","请在左侧边框选择插入的group");
        return;
    }
    qDebug()<<"insert into group:"<<article->group_id;

    article->user_id=user->user_id;
    char* temp1=new char[ui->lineEdit_2->text().toUtf8().length()+1];
    strcpy(temp1,ui->lineEdit_2->text().toUtf8().data());
    article->title=temp1;//怎么老是获取不到
    qDebug()<<"title"<<article->title;
    QByteArray s=ui->plainTextEdit->toPlainText().toUtf8().toBase64(QByteArray::Base64UrlEncoding | QByteArray::OmitTrailingEquals);
    article->text=s.data();
    //article->text=ui->plainTextEdit->toPlainText().toUtf8().toBase64(QByteArray::Base64UrlEncoding | QByteArray::OmitTrailingEquals).data();
    //article->text=ui->plainTextEdit->toPlainText().toUtf8().data();
    article->type=ui->comboBox->currentIndex()+1;
    char *buf=new char[13];
    memset(buf,'\0',13);
    short type=insert_article_t;//登录，返回User
    char* str=NULL;
    if(this->article!=NULL)
    {
        delete this->article;
        this->article=NULL;
    }
    this->article=article;
    //如果有对象数据要传输
    str=struct2json(article,ARTICLE,1);
    delete[] temp1;
    qDebug()<<str;
    int length=package_request2(buf,str,type);
    //发送
    tcpClient->write(buf,12); //qt5去除了.toAscii()
    tcpClient->write(str,length-12);
    tcpClient->waitForBytesWritten();
    tcpClient->flush();
}


/*
void MainWindow::on_action_7_triggered()
{
    //新建
    Article* article=new Article;
    QTreeWidgetItem* item=ui->treeWidget->currentItem();
    QString str_group=item->text(0);
    if(str_group.startsWith("id:"))
    {
        char* s=str_group.toLatin1().data();
        s=s+3;
        int num=0;
        for(int i=0;i<strlen(s);i++)
            qDebug()<<s[i]-'0';
        int i=0;
        while(s[i]!='\0'&&s[i]>='0'&&s[i]<='9')
        {
            num=num*10+(s[i]-'0');
            i++;
        }
        article->group_id=num;
    }
    else if(str_group.startsWith("user_id:"))
    {
        article->group_id=0;
    }
    else
    {
        QMessageBox::information(this,"新建文章无效","请在左侧边框选择插入的group");
        return;
    }
    qDebug()<<"insert into group:"<<article->group_id;

    article->user_id=user->user_id;
    article->title=ui->lineEdit_2->text().toUtf8().data();
    article->text=ui->plainTextEdit->toPlainText().toUtf8().data();
    article->type=ui->comboBox->currentIndex()+1;
    char *buf=new char[13];
    memset(buf,'\0',13);
    short type=insert_article_t;//登录，返回User
    char* str=NULL;
    if(this->article!=NULL)
    {
        delete this->article;
        this->article=NULL;
    }
    this->article=article;
    //如果有对象数据要传输
    str=struct2json(article,ARTICLE,1);
    qDebug()<<str;
    int length=package_request2(buf,str,type);
    //发送
    tcpClient->write(buf,12); //qt5去除了.toAscii()
    tcpClient->write(str,length-12);
    tcpClient->waitForBytesWritten();
    tcpClient->flush();
}
*/
void MainWindow::on_pushButton_9_clicked()
{
    if(ui->lineEdit_3->text()!=NULL)
    {
        //添加评论
        Comment* comment=new Comment;
        comment->com_user_id=user->user_id;
        comment->art_id=article->art_id;
        comment->text=ui->lineEdit_3->text().toUtf8().data();

        ui->listWidget->addItem(comment->text);
        char* str=struct2json(comment,COMMENT,1);
        char *buf=new char[13];
        memset(buf,'\0',13);
        short type=insert_comment_t;//登录，返回User
        int length=package_request2(buf,str,type);
        delete comment;
        //发送
        tcpClient->write(buf,12); //qt5去除了.toAscii()
        tcpClient->write(str,length-12);
        tcpClient->waitForBytesWritten();
        tcpClient->flush();
        delete[] buf;
        delete[] str;
    }
}

void MainWindow::on_download_triggered()
{
    //下载，保存到文件
    QString fileName(ui->lineEdit_2->text()+".txt") ;
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                      "/home",
                                                   QFileDialog::ShowDirsOnly
                                                      | QFileDialog::DontResolveSymlinks);
    QDir d;
    d.mkpath(dir);//可以不用，因为路径已经有了，就不用mk了
    QFile myfile(dir+"/"+fileName);
    if (myfile.open(QFile::WriteOnly|QFile::Truncate))//注意WriteOnly是往文本中写入的时候用，ReadOnly是在读文本中内容的时候用，Truncate表示将原来文件中的内容清空
    {
        //读取之前setPlainText的内容，或直接输出字符串内容QObject::tr()
        QTextStream out(&myfile);
        out<<ui->plainTextEdit->toPlainText()<<endl;
    }
}

void MainWindow::on_action_3_triggered()
{
    //更新
    //修改文章和文章名 类型
    article->user_id=user->user_id;
    //article->title=ui->lineEdit_2->text().toUtf8().data();
    //article->text=ui->plainTextEdit->toPlainText().toUtf8().toBase64(QByteArray::Base64UrlEncoding | QByteArray::OmitTrailingEquals).data();

    char* temp1=new char[ui->lineEdit_2->text().toUtf8().length()+1];
    strcpy(temp1,ui->lineEdit_2->text().toUtf8().data());
    article->title=temp1;//怎么老是获取不到
    qDebug()<<"title"<<article->title;
    QByteArray s=ui->plainTextEdit->toPlainText().toUtf8().toBase64(QByteArray::Base64UrlEncoding | QByteArray::OmitTrailingEquals);
    article->text=s.data();

    //article->text=ui->plainTextEdit->toPlainText().toUtf8().data();
    article->type=ui->comboBox->currentIndex()+1;
    ui->treeWidget->currentItem()->setText(1,ui->lineEdit_2->text());
    char* str=struct2json(article,ARTICLE,1);
    delete[] temp1;
    char *buf=new char[13];
    memset(buf,'\0',13);
    short type=modify_article_t;//登录，返回User
    int length=package_request2(buf,str,type);
    //delete article;
    //发送
    tcpClient->write(buf,12); //qt5去除了.toAscii()
    tcpClient->write(str,length-12);
    tcpClient->waitForBytesWritten();
    tcpClient->flush();
    delete[] buf;
    delete[] str;
    //不需要读取状态，或者简单读取返回成功与否
}

void MainWindow::on_action_4_triggered()
{
    //删除文章
    //成员article保存当前的article信息
    Article* article=new Article;
    article->art_id=ui->treeWidget->currentItem()->text(0).toInt();
    if(article->art_id==0)
    {
        QMessageBox::information(this,"删除文章失败","请选中想要删除的文章\n");
        delete article;
        return;
    }
    qDebug()<<"删除的文章id"<<article->art_id;
    article->user_id=this->user->user_id;
    char* str=struct2json(article,ARTICLE,1);
    char *buf=new char[100];
    memset(buf,'\0',100);
    short type=delete_article_t;//登录，返回User
    int length=package_request(buf,str,type);
    delete article;
    //发送
    tcpClient->write(buf,length); //qt5去除了.toAscii()
    tcpClient->waitForBytesWritten();
    tcpClient->flush();
    delete[] buf;

    QTreeWidget* unitTree=ui->treeWidget;
    QTreeWidgetItemIterator iterator(unitTree);
    while (*iterator){
        if ((*iterator)->text(0)==QString::number(article->art_id)) {
            //找到目标 item ，做相关操作
            QTreeWidgetItem *item = (*iterator);
            item->parent()->removeChild(item);
            break;
        }
        ++iterator;
    }
    ui->lineEdit_2->clear();
    ui->plainTextEdit->clear();
    ui->listWidget->clear();
}


int MainWindow::get_group()
{
    QTreeWidgetItem* item=ui->treeWidget->currentItem();
    if(item==NULL)
        return 0;
    QString str=item->text(0);
    if(str.startsWith("user_id"))
    {
        return 0;
    }
    else if(str.startsWith("id:"))
    {
        char* s=str.toLatin1().data();
        s=s+3;
        int num=0;
        for(unsigned int i=0;i<strlen(s);i++)
            qDebug()<<s[i]-'0';
        int i=0;
        while(s[i]!='\0'&&s[i]>='0'&&s[i]<='9')
        {
            num=num*10+(s[i]-'0');
            i++;
        }
        return num;
    }
    else
    {
        return -1;
    }
}
static int group_father=0;
QString group_name;
void MainWindow::update_new_group(Group *group)
{
    if(group!=NULL)
    {
        QTreeWidget* unitTree=ui->treeWidget;
        QTreeWidgetItemIterator iterator(unitTree);
        while (*iterator){
            if ((*iterator)->text(0)==QString("id:")+QString::number(group_father)) {
                //找到目标 item ，做相关操作
                QTreeWidgetItem *item = (*iterator);
                QTreeWidgetItem *new_group=new QTreeWidgetItem(QStringList()<<QString("id:")+QString::number(group->group_id)<<group_name);
                item->addChild(new_group);
                break;
            }
            ++iterator;
        }
        if(group_father==0)
        {
            QTreeWidgetItem *art=new QTreeWidgetItem(QStringList()<<QString("id:")+QString::number(group->group_id)<<group_name);
            ui->treeWidget->topLevelItem(0)->addChild(art);//不应该写死
        }
        delete group;
    }
}

void MainWindow::delete_group()
{
    QTreeWidgetItem *x=ui->treeWidget->currentItem();
    x->parent()->removeChild(x);
    QMessageBox::information(this,"删除分组成功","删除分组成功");
}
void MainWindow::on_toolButton_clicked()
{
    //加组
    Group* group=new Group;
    group_father=get_group();
    group->father_group_id=group_father;
    qDebug()<<"select father_group_id ="<<group->father_group_id;
    if(group->father_group_id==-1)
    {
        QMessageBox::information(this,"插入分组失败","请选中父分组");
        delete group;
        return;
    }
    group->user_id=user->user_id;

    QString dlgTitle="插入分组";
    QString txtLabel="请输入新分组的名字";
    QString defaultInput="";
    QLineEdit::EchoMode echoMode=QLineEdit::Normal;//正常文字输入
    bool ok=false;
    QString text = QInputDialog::getText(this, dlgTitle,txtLabel, echoMode,defaultInput, &ok);
    if (ok && text!="")
    {
        QString temp1=text;
        group->group_name=temp1.toUtf8().data();
        group_name=group->group_name;
        //ui->listWidget->addItem();

        char* str=struct2json(group,GROUP,1);
        qDebug()<<str;
        char *buf=new char[500];
        memset(buf,'\0',500);
        short type=insert_group_t;//登录，返回User
        int length=package_request(buf,str,type);
        delete group;
        //发送
        tcpClient->write(buf,length); //qt5去除了.toAscii()
        tcpClient->waitForBytesWritten();
        tcpClient->flush();
        delete[] buf;
    }
}

void MainWindow::on_toolButton_2_clicked()
{
    //减组
    Group* group=new Group;
    group->group_id=get_group();
    if(group->group_id==0)
    {
        QMessageBox::information(this,"删除分组失败","不能删除用户主分组或非分组类型成员");
        delete group;
        return;
    }
    if(group->group_id!=-1)
    {
        group->user_id=user->user_id;
        char* str=struct2json(group,GROUP,1);
        qDebug()<<str;
        char *buf=new char[500];
        memset(buf,'\0',500);
        short type=delete_group_t;//登录，返回User
        int length=package_request(buf,str,type);
        delete group;
        //发送
        tcpClient->write(buf,length); //qt5去除了.toAscii()
        tcpClient->waitForBytesWritten();
        tcpClient->flush();
        delete[] buf;
    }
    else
    {
        QMessageBox::information(this,"删除分组失败","不能删除用户主分组或非分组类型成员");
        delete group;
        return;
    }
}
void MainWindow::get_type_group(int* type,int *id)
{
    QTreeWidgetItem *item=ui->treeWidget->currentItem();
    QString str=item->text(0);
    if(item==NULL)
    {
        *type=0;
        *id=0;
        return;
    }
    else if(str.startsWith("id:"))
    {
            char* s=str.toUtf8().data();
            s=s+3;
            int num=0;
            for(unsigned int i=0;i<strlen(s);i++)
                qDebug()<<s[i]-'0';
            int i=0;
            while(s[i]!='\0'&&s[i]>='0'&&s[i]<='9')
            {
                num=num*10+(s[i]-'0');
                i++;
            }
            *type=1;
            *id=num;
    }
    else if(item->text(0).startsWith("user_id"))
    {
        *type=0;
        *id=0;
    }
    else
    {
        *type=2;
        *id=str.toInt();
    }
}

void MainWindow::modify_father_group()
{
    QTreeWidgetItem* item_f=ui->treeWidget->currentItem();
    item_f->parent()->removeChild(item_f);
    //遍历
    QTreeWidget* unitTree=ui->treeWidget;
    QTreeWidgetItemIterator iterator(unitTree);
    while (*iterator){
        if ((*iterator)->text(0)==QString("id:")+QString::number(group_now)){
            //找到目标 item ，做相关操作
            QTreeWidgetItem *item = (*iterator);
            item->addChild(item_f);
            break;
        }
        ++iterator;
    }
    if(group_now==0)
    {
        ui->treeWidget->topLevelItem(0)->addChild(item_f);//不应该写死
    }
}

void MainWindow::modify_article_group()
{
    QTreeWidgetItem* item_f=ui->treeWidget->currentItem();
    item_f->parent()->removeChild(item_f);
    //遍历
    QTreeWidget* unitTree=ui->treeWidget;
    QTreeWidgetItemIterator iterator(unitTree);
    while (*iterator){
        if ((*iterator)->text(0)==QString("id:")+QString::number(group_now)){
            //找到目标 item ，做相关操作
            QTreeWidgetItem *item = (*iterator);
            item->addChild(item_f);
            break;
        }
        ++iterator;
    }
    if(group_now==0)
    {
        ui->treeWidget->topLevelItem(0)->addChild(item_f);//不应该写死
    }
}

void MainWindow::on_toolButton_3_clicked()
{
    //换分组、文章的父分组。
    int type,id;
    get_type_group(&type,&id);
    qDebug()<<"id:"<<id;
    if(type==0)
    {
        QMessageBox::information(this,"修改分组失败","未选中文章或分组");
        return;
    }
    else if(type==1)//分组
    {
        QString dlgTitle="输入分组";
        QString txtLabel="请输入分组想要修改到的父分组id";
        QString defaultInput="";
        QLineEdit::EchoMode echoMode=QLineEdit::Normal;//正常文字输入
        bool ok=false;
        QString text = QInputDialog::getText(this,dlgTitle,txtLabel,echoMode,defaultInput,&ok);
        if (ok && text.toInt()!=0)
        {
            Group* group=new Group;
            group->user_id=my_user->user_id;
            group->father_group_id=text.toInt();
            group_now=group->father_group_id;
            group->group_id=id;
            if(group->father_group_id==group->group_id)
            {
                QMessageBox::information(this,"修改分组失败","修改分组失败1");
                delete group;
                return;
            }
            if(group->group_id==0)
            {
                QMessageBox::information(this,"修改分组失败","修改分组失败1");
                delete group;
                return;
            }
            if(group->group_id!=-1)
            {
                //对话框获取father_id，group_name。

                group->user_id=user->user_id;
                char* str=struct2json(group,GROUP,1);
                qDebug()<<str;
                char *buf=new char[500];
                memset(buf,'\0',500);
                short type=modify_group_t;//登录，返回User
                int length=package_request(buf,str,type);
                delete group;
                //发送
                tcpClient->write(buf,length); //qt5去除了.toAscii()
                tcpClient->waitForBytesWritten();
                tcpClient->flush();
                delete[] buf;
            }
            else
            {
                QMessageBox::information(this,"修改分组失败","修改分组失败2");
                delete group;
                return;
            }
        }
        else if(ok)
        {
            QMessageBox::information(this,"修改分组失败","修改分组失败2");
            delete group;
            return;
        }
    }
    else if(type==2)//文章
    {
        QString dlgTitle="输入分组";
        QString txtLabel="请输入文章想要修改到的分组id";
        QString defaultInput="";
        QLineEdit::EchoMode echoMode=QLineEdit::Normal;//正常文字输入
        bool ok=false;
        QString text = QInputDialog::getText(this,dlgTitle,txtLabel,echoMode,defaultInput,&ok);
        if (ok && text.toInt()!=0)
        {
            Article* article=new Article;
            article->user_id=my_user->user_id;
            article->group_id=text.toInt();
            article->art_id=id;
            group_now=article->group_id;
            char* str=struct2json(article,ARTICLE,1);
            char *buf=new char[500];
            memset(buf,'\0',500);
            short type=modify_article_group_t;//登录，返回User
            int length=package_request(buf,str,type);
            //delete article;
            //发送
            tcpClient->write(buf,length);
            tcpClient->waitForBytesWritten();
            tcpClient->flush();
            delete[] buf;
        }
        else if(ok)
        {
            QMessageBox::information(this,"修改分组失败","请输入正确的分组id");
        }
        //对话框获取group
        //article->group_id=group;
    }
}

void MainWindow::on_toolButton_4_clicked()
{
    //删除评论
    if(ui->listWidget->currentItem()==NULL)
        return;
    Comment* comment=new Comment;
    QString str=ui->listWidget->currentItem()->text();
    char* s=str.toUtf8().data();
    int num=0;
    int i=0;
    while(s[i]!='\0'&&s[i]>='0'&&s[i]<='9')
    {
        num=num*10+(s[i]-'0');
        i++;
    }
    comment->comment_id=num;
    if(comment->comment_id==0)
    {
        QMessageBox::information(this,"删除评论失败","删除评论失败");
        delete comment;
        return;
    }
    else
    {
        char* str=struct2json(comment,COMMENT,1);
        qDebug()<<str;
        char *buf=new char[500];
        memset(buf,'\0',500);
        short type=delete_comment_t;//登录，返回User
        int length=package_request(buf,str,type);
        delete comment;
        //发送
        tcpClient->write(buf,length); //qt5去除了.toAscii()
        tcpClient->waitForBytesWritten();
        tcpClient->flush();
        delete[] buf;
        ui->listWidget->takeItem(ui->listWidget->currentRow());
    }
}
