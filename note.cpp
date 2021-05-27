#include "note.h"

note::note()
{
    /*   if(status==success)
           New *win=new New;可以在构造函数中传结构体指针User
           win->show();
           win->init();
           this->close();
         else(status==Failure)
            QMessageBox::information(this,"登录错误","账号或者密码错误");
    */

//    QDialog dlg(this);
//    dlg.resize(200,100);
//    dlg.exec();
//    //qDebug<<"模态对话框";
//    dlg.setAttribute(Qt::WA_DeleteOnClose);


    //每一次对按钮的点击都要用到网络
    /*
     * 破坏了封装性，以后重新设计，使用智能指针，RAII
     *
     */

    /*
    //槽函数。每次接收数据
    QByteArray buffer;//成员函数
    void Tcp_net::ReadData()
    {
        if(length==0)//第一次
        {

            buffer = tcpClient->readAll(4);
            length=trans4((buffer.data()));//协议的前四字节是包总长度
            qDebug()<<"取得头部length:"<<QString::number(length);
        }
        if(length-4>tcpClient->bytesAvailable())//第二、三。。次
        {
            qDebug()<<"可读数据不足一个包";
            return;
        }
        else if(length!=0)
        {
            //协议层
            buffer=tcpClient->read(length-4);//读出除了前四字节以外的数据
            length=0;//保证下一次读取有效
            qDebug()<<"开始处理一个响应";
        }



        //每次发送请求
        tcpClient->write(buf,length); //qt5去除了.toAscii()
        tcpClient->waitForBytesWritten();
        tcpClient->flush();

}

    length=trans4((buffer.data()));
    qDebug()<<"取得头部length:"<<QString::number(length);
}
if(length-4>tcpClient->bytesAvailable())//第二、三。。次
{
    qDebug()<<"不足一个包";
    return;
}
else if(length!=0)
{
    //协议层
    qDebug()<<"处理一个响应";
    buffer=tcpClient->read(length-4);
    int len=length;
    length=0;//保证下一次读取有效

QByteArray buffer;
if(length==0&&tcpClient->bytesAvailable()>=4)//第一次
{
    //buffer = tcpClient->read(4);
    //int x=tcpClient->read(m_buffer,10000);
    length=trans4((buffer.data()));
    qDebug()<<"取得头部length:"<<QString::number(length);
}
if(length-4>tcpClient->bytesAvailable())//第二、三。。次
{
    qDebug()<<"不足一个包";
    return;
}
else if(length!=0)
{
    //协议层
    qDebug()<<"处理一个响应";
    buffer=tcpClient->read(length-4);
    int len=length;
    length=0;//保证下一次读取有效

    int status=trans2((buffer.data()));
    int type=trans2((buffer.data()+2));
    int CRC=trans4((buffer.data()+4));//暂无
    qDebug()<<"Response:        "<<"status:"<<status<<" type:"<<type<<" length:"<<len;
    if(len>12)
        qDebug()<<"json: "<<buffer.data()+8;
    else
        qDebug()<<"json:"<<"NULL";
    CRC++;
    if(status==SUCCESS&&len>=12)
    {
        int size=0;
        switch(type)
        {
        case login_t:
        {
            User* user=NULL;
            user=(User*)json2struct(buffer.data()+8,USER,&size);
            if(user==NULL)
                QMessageBox::information(this,"无账号信息","无账号信息");
            else
            {
                //new_win->update(user);
                if(new_win==NULL)
                {
                    new_win=new New;
                    new_win->setfather(this->father);
                    new_win->tcpClient=this->tcpClient;//key!!此处将tcpClient传给new Window
                    //提前生成mainwindow，以备关注，收藏跳转
                    if(mainwindow==NULL)
                    {
                        mainwindow=new MainWindow;
                        mainwindow->tcpClient=tcpClient;
                        new_win->mainwindow=mainwindow;
                        mainwindow->setfather(new_win);
                    }
                    new_win->set_User(user);
                    mainwindow->user=new_win->user;
                    mainwindow->my_user=new_win->user;
                }
                new_win->show();
                new_win->father->hide();
            }
            break;
        }
        case query_user_t:
        {
            User* user=NULL;
            user=(User*)json2struct(buffer.data()+8,USER,&size);
            if(user!=NULL)
            {
                if(new_win->user->user_id==user->user_id)
                {
                    new_win->set_User(user);//重新获取了本人信息
                    mainwindow->user=user;
                    mainwindow->my_user=user;
                }
                form=new_win->form;
                form->user=user;
                form->update();
            }
            break;
        }
        case query_user_rel_t:
        {
            new_win->focus->mainwindow=this->mainwindow;
            new_win->focus->tcpClient=tcpClient;
            User_Relation* rel=(User_Relation*)json2struct(buffer.data()+8,USER_REL,&size);
            if(rel!=NULL)
            {
                focus=this->new_win->focus;
                focus->update(rel,size);
            }
            break;
        }
        case query_collect_t:
        {
            new_win->col->mainwindow=this->mainwindow;
            new_win->col->tcpClient=tcpClient;
            Collect* collect=(Collect*)json2struct(buffer.data()+8,COLLECT,&size);
            if(collect!=NULL)
            {
                col=this->new_win->col;
                col->update(collect,size);
            }
            break;
        }
        case query_group_t:
        {
            //有时候查不出来的原因是第二个包先到了
            Group* group=NULL;
            group=(Group*)json2struct(buffer.data()+8,GROUP,&size);
            if(group!=NULL)
            {
                mainwindow->update_group(group,size);
                //对文章标题列表查询
            }
            else
            {
                mainwindow->update_group(NULL,0);
            }
            //查文章标题
            char *buf=new char[500];
            char* str=NULL;
            int length=0;
            memset(buf,'\0',500);
            type=query_article_title_t;//登录，返回User
            str=NULL;
            Article* article=new Article;
            article->user_id=mainwindow->user->user_id;
            //如果有对象数据要传输
            str=struct2json(article,ARTICLE,1);
            delete article;
            length=package_request(buf,str,type);

            //发送
            tcpClient->write(buf,length); //qt5去除了.toAscii()
            tcpClient->flush();
            delete[] buf;
            break;
        }
        case query_article_title_t:
        {
            Article* article=NULL;
            article=(Article*)json2struct(buffer.data()+8,ARTICLE,&size);
            if(article!=NULL)
            {
                mainwindow=this->new_win->mainwindow;
                mainwindow->update_article_list(article,size);
                tcpClient->flush();
            }
            break;
        }
        case query_article_t:
        {
            Article* article=NULL;
            article=(Article*)json2struct(buffer.data()+8,ARTICLE,&size);
            if(article!=NULL)
            {
                mainwindow=this->new_win->mainwindow;
                mainwindow->update_article(article);
            }
            break;
        }
        case query_comment_t:
        {
            Comment* comment=NULL;
            if(len>12)
                comment=(Comment*)json2struct(buffer.data()+8,COMMENT,&size);
            else
                size=0;
                mainwindow=this->new_win->mainwindow;
                mainwindow->update_comment(comment,size);
                tcpClient->flush();
            break;
        }
        case insert_article_t:
        {
            QMessageBox::information(this,"插入文章成功","插入文章成功");
            if(len>12)
            {
                Article* article=NULL;
                article=(Article*)json2struct(buffer.data()+8,ARTICLE,&size);
                if(article!=NULL)
                {
                    mainwindow->insert_article(article,1);
                    tcpClient->flush();
                }
            }
            break;
        }
        case modify_article_t:
        {
            QMessageBox::information(this,"修改文章成功","修改文章成功");
            break;
        }
        case delete_article_t:
        {
            //更新左侧界面，右侧评论和文章等
            break;
        }
        case modify_user_t:
        {
            //修改user
            QMessageBox::information(this,"修改个人信息成功","修改个人信息成功");
            break;
        }
        case query_user_rel_simple_t:
        {
            //查询是否存在关注关系 state=SUCCESS
            mainwindow->set_rel_on();
            break;
        }
        case query_collect_simple_t:
        {
            //查询是否存在关注关系 state=SUCCESS
            mainwindow->set_col_on();
            break;
        }
        case insert_user_rel_t:
        {
            break;
        }
        case delete_user_rel_t:
        {
            break;
        }
        case insert_collect_t:
        {
            break;
        }
        case delete_collect_t:
        {
            break;
        }
        case add_art_upvote_t:
        {
            //无操作
            break;
        }
        case query_article_bytype_t:
        {
            Article* article=NULL;
            article=(Article*)json2struct(buffer.data()+8,ARTICLE,&size);
            if(article!=NULL)
            {
                forum=this->new_win->forum;
                forum->update_article_bytype(article,size);
                tcpClient->flush();
            }
            break;
        }
        case query_art_bynow_t:
        {
            Article* article=NULL;
            article=(Article*)json2struct(buffer.data()+8,ARTICLE,&size);
            if(article!=NULL)
            {
                forum=this->new_win->forum;
                forum->update_article_bynow(article,size);
                tcpClient->flush();
            }
            break;
        }
        case query_art_byweek_t:
        {
            Article* article=NULL;
            article=(Article*)json2struct(buffer.data()+8,ARTICLE,&size);
            if(article!=NULL)
            {
                forum=this->new_win->forum;
                forum->update_article_byweek(article,size);
                tcpClient->flush();
            }
            break;
        }
        case query_art_bymonth_t:
        {
            Article* article=NULL;
            article=(Article*)json2struct(buffer.data()+8,ARTICLE,&size);
            if(article!=NULL)
            {
                forum=this->new_win->forum;
                forum->update_article_bymonth(article,size);
                tcpClient->flush();
            }
            break;
        }
        case insert_user_t:
        {
            QMessageBox::information(this,"注册成功","注册成功");
            break;
        }
        }
    }
    else if(status==FAILURE)
    {
        if(type==login_t)
            QMessageBox::information(this,"登录错误","账号或者密码错误");
        else if(type==insert_user_t)
            QMessageBox::information(this,"注册失败","该账号已存在");
        else if(type==query_group_t)
        {
            mainwindow->update_group(NULL,0);

            char *buf=new char[500];
            char* str=NULL;
            int length=0;
            memset(buf,'\0',500);
            type=query_article_title_t;//登录，返回User
            str=NULL;
            Article* article=new Article;
            article->user_id=mainwindow->user->user_id;
            //如果有对象数据要传输
            str=struct2json(article,ARTICLE,1);
            delete article;
            length=package_request(buf,str,type);
            qDebug()<<length<<"  "<<buf+12;
            //发送
            tcpClient->write(buf,length); //qt5去除了.toAscii()
            delete[] buf;
            tcpClient->waitForBytesWritten();
            tcpClient->flush();

            //是否其他人的博客
            mainwindow->init_other_blog();
        }
        else if(type==query_user_rel_simple_t)
        {
            mainwindow->set_rel_down();
        }
        else if(type==query_collect_simple_t)
        {
            mainwindow->set_col_down();
        }
        else if(type==add_art_upvote_t)
        {
        }
        else if(type==insert_user_rel_t)
        {
        }
        else if(type==delete_user_rel_t)
        {
        }
        else if(type==query_collect_simple_t)
        {
        }
        else if(type==insert_collect_t)
        {
        }
        else if(type==delete_collect_t)
        {
        }
        else if(type==query_article_bytype_t)
        {
        }
        else if(type==query_art_bynow_t)
        {
        }
        else if(type==query_art_byweek_t)
        {
        }
        else if(type==query_art_bymonth_t)
        {
        }
    }
    else
    {}
}
}

Tcp_net::~Tcp_net()
{
tcpClient->disconnectFromHost();
if (tcpClient->state() == QAbstractSocket::UnconnectedState \
                || tcpClient->waitForDisconnected(1000))
{
    qDebug()<<"关闭连接";
}
tcpClient->close();
delete tcpClient;
delete ui;
}

void Tcp_net::setfather(QWidget *father)
{
this->father=father;
}
*/
}
