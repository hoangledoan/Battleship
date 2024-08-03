#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

//#define test_main_ui

MainWindow::MainWindow(QWidget *parent,std::shared_ptr<kbm::State> shared_state, std::shared_ptr<Game> game )
    : QMainWindow(parent)
    , missed_animation(std::make_shared<QMovie>(this)), hitted_animation(std::make_shared<QMovie>(this)),
    destroyed_animation(std::make_shared<QMovie>(this)), ui(new Ui::MainWindow),
    mode_select(std::make_unique<QMenu>(this)), ship_input(std::make_unique<QMenu>(this)), rotation(std::make_unique<QMenu>(this)),
    shared_state(shared_state), current_game(game)
//    ,sound_track(std::make_unique<QMediaPlayer>(this))
{
    //main_ui cant change it size for asthetic reason, on recall this function will be called;
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);
    main_ui_config();
    //Add Menu for server_client menu and ship intake menu
    menu_setup();

    missed_animation->setFileName(":/Img_file/missed.gif");
    hitted_animation->setFileName(":/Img_file/elmo-burning.gif");
    destroyed_animation->setFileName(":/Img_file/explosion.gif");

//    sound_track->setSource(QUrl(":/sound_track.mp3"));
//    sound_track->play();

    missed_animation->start();
    hitted_animation->start();
    destroyed_animation->start();

    ui->schlachtschiff_left->setEnabled(false);
    ui->kreuzer_left->setEnabled(false);
    ui->destroyer_left->setEnabled(false);
    ui->uboot_left->setEnabled(false);

    connect(ui->ship_input,&QLineEdit::textEdited,this,[&](){
        if(init_stage.on_edit==zustand::ship_kind::undefined)   return;

        (init_stage.ship_list)[init_stage.on_edit]= ui->ship_input->text().toInt();
    });

//    connect(sound_track.get(), &QMediaPlayer::mediaStatusChanged, sound_track.get(),[=]{

//        if(sound_track->mediaStatus()== QMediaPlayer::StoppedState)
//        {
//            sound_track->play();
//        }

//    });
}


MainWindow::~MainWindow()
{
    delete ui;

    for(auto& button : button_container){
        delete button;
    }
}

void MainWindow::on_start_clicked()
{
    shared_state->client_mode= init_stage.op_mode== zustand::SERVER_CLIENT::_server? kbm::State::ClientMode::Server :
                                    kbm::State::ClientMode::Client;

    if(init_stage.op_mode== zustand::SERVER_CLIENT::_server)
    {
        init_stage.play_field.width= ui->field_width->text().toInt() ==0 ? 10 : ui->field_width->text().toInt();
        init_stage.play_field.height= ui->field_height->text().toInt()== 0 ? 10 : ui->field_height->text().toInt();

        shared_state->width= ui->field_width->text().toInt() ==0 ? 10 : ui->field_width->text().toInt();
        shared_state->height= ui->field_height->text().toInt()== 0 ? 10 : ui->field_height->text().toInt();

        shared_state->n_battleships= init_stage.ship_list[zustand::ship_kind::Schlachtschiff];
        shared_state->n_cruisers= init_stage.ship_list[zustand::ship_kind::Kreuzer];
        shared_state->n_destroyers= init_stage.ship_list[zustand::ship_kind::Zerstoerer];
        shared_state->n_submarines= init_stage.ship_list[zustand::ship_kind::UBoot];

        shared_state->host = ui->setup_ip->text() == "" ? "localhost" : ui->setup_ip->text().toStdString();
        shared_state->port = ui->setup_port->text().toInt() == 0 ? 8080 : ui->setup_port->text().toInt();

        current_game->set_field(init_stage.play_field.width,init_stage.play_field.height);

        wait_setup();
    }

    else
    {
        // ip_defautlt change accordingly
        shared_state->host = ui->setup_ip->text() == "" ? "localhost" : ui->setup_ip->text().toStdString();
        shared_state->port = ui->setup_port->text().toInt() == 0 ? 8080 : ui->setup_port->text().toInt();
    }

    shared_state->start_client_or_server= true;

    emit to_set_connection();
}

void MainWindow::on_back_clicked()
{
    main_ui_config();

    ui->stackedWidget->setCurrentIndex(0);

    current_game->reset_game();

    shared_state->disconnect= true;

    emit abort();
}

void MainWindow::on_back2_clicked(){
    main_ui_config();

    current_game->reset_game();

    ui->stackedWidget->setCurrentIndex(0);

    shared_state->disconnect= true;

    emit abort();
}

void MainWindow::forced_abort()
{
    main_ui_config();

    current_game->reset_game();

    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::menu_setup()
{

    ui->play_field->setGeometry(QRect(0,0,PLAY_FIELD_PIX_WIDTH, PLAY_FIELD_PIX_HEIGHT));
    ui->play_menu->setGeometry(QRect(521,0,PLAY_UI_WIDTH-PLAY_FIELD_PIX_WIDTH-20, PLAY_FIELD_PIX_HEIGHT));

    mode_select->addAction("Server",[&](){
        init_stage.op_mode= zustand::SERVER_CLIENT::_server;
        ui->mode->setText("Server");
    });

    mode_select->addAction("Client",[&](){
        init_stage.op_mode= zustand::SERVER_CLIENT::_client;
        ui->mode->setText("Client");
    });

    rotation->addAction("Horizontal",[&](){
        init_stage.or_mode= zustand::Orientation::_horizontal;
        ui->Rotation->setText("Horizontal");
    });
    rotation->addAction("Vertical",[&](){
        init_stage.or_mode= zustand::Orientation::_vertical;
        ui->Rotation->setText("Vertical");
    });

    ship_input->addAction("Schlachtschiff",[this]{
        init_stage.on_edit=zustand::ship_kind::Schlachtschiff;

        if(ui->stackedWidget->currentIndex() == 0){
            ui->ship_list->setText("Schlachtschiff");
            ui->ship_input->setText(QString::number(init_stage.ship_list[zustand::ship_kind::Schlachtschiff]));
        }

        else{
            ui->choose_ship->setText("Schlachtschiff");
        }
    });

    ship_input->addAction("Kreuzer",[this]{
        init_stage.on_edit=zustand::ship_kind::Kreuzer;

        if(ui->stackedWidget->currentIndex() == 0){
            ui->ship_list->setText("Kreuzer");
            ui->ship_input->setText(QString::number(init_stage.ship_list[zustand::ship_kind::Kreuzer]));
        }

        else{
            ui->choose_ship->setText("Kreuzer");
        }
    });

    ship_input->addAction("Zerstoerer",[this]{
        init_stage.on_edit=zustand::ship_kind::Zerstoerer;

        if(ui->stackedWidget->currentIndex() == 0){
            ui->ship_list->setText("Zerstoerer");
            ui->ship_input->setText(QString::number(init_stage.ship_list[zustand::ship_kind::Zerstoerer]));
        }

        else{
            ui->choose_ship->setText("Zerstoerer");
        }
    });
    ship_input->addAction("U-Boot",[this]{
        init_stage.on_edit=zustand::ship_kind::UBoot;

        if(ui->stackedWidget->currentIndex() == 0){
            ui->ship_list->setText("U-Boot");
            ui->ship_input->setText(QString::number(init_stage.ship_list[zustand::ship_kind::UBoot]));
        }

        else{
            ui->choose_ship->setText("U-Boot");
        }

    });

    //import menu into button
    ui->mode->setMenu(mode_select.get());
    ui->ship_list->setMenu(ship_input.get());
    ui->Rotation->setMenu(rotation.get());
}

void MainWindow::main_ui_config()
{
    //    this->ui->setGeometry(0,0,580,500);

    for(auto& button : button_container){
        ui->play_field->removeWidget(button);
        delete button;
    }

    button_container.shrink_to_fit();
    button_container.clear();

    for(auto& button : game_button_container){
        ui->play_f->removeWidget(button);
        delete button;
    }

    game_button_container.shrink_to_fit();
    game_button_container.clear();

    get_enemy.clear();

    missed_animation.get()->disconnect();
    hitted_animation.get()->disconnect();
    destroyed_animation.get()->disconnect();

    this->ui->field_width->setText("10");
    this->ui->field_height->setText("10");

    this->ui->field_width->setPlaceholderText("10");
    this->ui->field_height->setPlaceholderText("10");

    //
    this->ui->setup_ip->setText("localhost");
    this->ui->setup_port->setText("8080");

    this->ui->setup_ip->setPlaceholderText("localhost");
    this->ui->setup_port->setPlaceholderText("8080");

    init_stage.ship_list= std::map<zustand::ship_kind, uint16_t>
        {{zustand::ship_kind::Schlachtschiff,1},{zustand::ship_kind::Kreuzer,2},{zustand::ship_kind::Zerstoerer,3},{zustand::ship_kind::UBoot,4}};

    this->setFixedSize(MAIN_UI_WIDTH,MAIN_UI_HEIGHT);
}


void MainWindow::wait_setup()
{

    ui->stackedWidget->setCurrentIndex(1);

#ifdef test_main_ui
    qDebug() << init_stage.play_field.width << init_stage.play_field.height;
    for(auto x = init_stage.ship_list.begin(); x!=init_stage.ship_list.end(); x++ )
    {
        qDebug()<<x->second;
        qDebug()<<shared_state->width<<shared_state->height<<shared_state->n_battleships<<shared_state->n_cruisers;
    }

#endif
    this->setFixedSize(PLAY_UI_WIDTH,PLAY_UI_HEIGHT);

    ui->stackedWidget->setBaseSize(PLAY_UI_WIDTH,PLAY_UI_HEIGHT);

    ui->play_field->setGeometry(QRect(0,0,PLAY_FIELD_PIX_WIDTH,PLAY_FIELD_PIX_HEIGHT));

    QSize btnSize= QSize(static_cast<int>(PLAY_FIELD_PIX_WIDTH/init_stage.play_field.width)-12,
                          static_cast<int>(PLAY_FIELD_PIX_HEIGHT/init_stage.play_field.height)-12
                          );
    for(int i =0 ; i < init_stage.play_field.height;i++)
    {
        for(int j =0 ; j < init_stage.play_field.width;j++)
        {
            auto button = new QPushButton(this);
            button->setFixedSize(btnSize);
            button->setIconSize(btnSize);
            button->setMaximumHeight(i*init_stage.play_field.width+j+10000);
            button->setStyleSheet("background: rgb(255, 255, 255);");
            ui->play_field->addWidget(button, i, j);

            ui->play_field->setSpacing(0);

            button_container.push_back(button);

            connect(button, &QPushButton::clicked , this, &MainWindow::button_play_field_clicked);
        }
    }
    ui->choose_ship->setMenu(ship_input.get());

    //display
    ui->schlachtschiff_left->setText(QString::number(init_stage.ship_list[zustand::ship_kind::Schlachtschiff]));
    ui->kreuzer_left->setText(QString::number(init_stage.ship_list[zustand::ship_kind::Kreuzer]));
    ui->destroyer_left->setText(QString::number(init_stage.ship_list[zustand::ship_kind::Zerstoerer]));
    ui->uboot_left->setText(QString::number(init_stage.ship_list[zustand::ship_kind::UBoot]));

}

void MainWindow::update_wait_ui()
{
    //update display upon clicked
    ui->schlachtschiff_left->setText(QString::number(init_stage.ship_list[zustand::ship_kind::Schlachtschiff]));
    ui->kreuzer_left->setText(QString::number(init_stage.ship_list[zustand::ship_kind::Kreuzer]));
    ui->destroyer_left->setText(QString::number(init_stage.ship_list[zustand::ship_kind::Zerstoerer]));
    ui->uboot_left->setText(QString::number(init_stage.ship_list[zustand::ship_kind::UBoot]));

}

void MainWindow::button_play_field_clicked()
{
    QPushButton* pushed_button=qobject_cast<QPushButton*>(sender());

    init_stage.pos_pushed= pushed_button->maximumHeight()-10000;

    //    init_stage.pushed_button= pushed_button;

    if(ui->stackedWidget->currentIndex()==1){
        if(init_stage.ship_list[init_stage.on_edit]>0){
            //pushed_button->setStyleSheet("background: rgb(0, 0, 255);font: 1px; color:rgb(0, 0, 255);");

            update_wait_ui();

            emit check_placement();
        }
    }

    else{

        if(current_game->update_enemy_position(init_stage.pos_pushed))
        {
            shared_state->shoot= true;

            current_game->convert_to_xy(init_stage.pos_pushed, shared_state->shoot_x, shared_state->shoot_y);

            emit shot_fired();
        }
    }
}

void MainWindow::on_play_clicked()
{

#ifndef test_main_ui
    bool check_placed_all_ship= true;
    for(auto x = init_stage.ship_list.begin(); x!=init_stage.ship_list.end(); x++){
        if(x->second!=0) check_placed_all_ship=false;
    }
    if(check_placed_all_ship)
    {
        shared_state->ready_to_begin= true;

        emit ready_to_play();
    }
#endif
#ifdef test_main_ui

    start_game();
#endif
}

void MainWindow::start_game()
{
    ui->stackedWidget->setCurrentIndex(2);

    ui->stackedWidget->setBaseSize(PLAY_UI_WIDTH,PLAY_UI_HEIGHT);

    ui->play_f->setGeometry(QRect(0,0,PLAY_FIELD_PIX_WIDTH,PLAY_FIELD_PIX_HEIGHT));


    QSize btnSize= QSize(static_cast<int>(PLAY_FIELD_PIX_WIDTH/init_stage.play_field.width)-12,
                          static_cast<int>(PLAY_FIELD_PIX_HEIGHT/init_stage.play_field.height)-12);

    for(int i =0 ; i < init_stage.play_field.height;i++)
    {
        for(int j =0 ; j < init_stage.play_field.width;j++)
        {
            auto button = new QPushButton(this);
            button->setFixedSize(btnSize);
            button->setIconSize(btnSize);
            button->setMaximumHeight(i*init_stage.play_field.width+j+10000);

            button->setStyleSheet("background: rgb(255, 255, 255);");
            ui->play_f->addWidget(button, i, j);

            ui->play_f->setSpacing(0);

            game_button_container.push_back(button);

            connect(button, &QPushButton::clicked , this, &MainWindow::button_play_field_clicked);
        }
    }
}

void MainWindow::get_play_parameters()
{
    init_stage.play_field.width=shared_state->width;
    init_stage.play_field.height= shared_state->height;

    init_stage.ship_list[zustand::ship_kind::Schlachtschiff]=shared_state->n_battleships;
    init_stage.ship_list[zustand::ship_kind::Kreuzer]=shared_state->n_cruisers;
    init_stage.ship_list[zustand::ship_kind::Zerstoerer]=shared_state->n_destroyers;
    init_stage.ship_list[zustand::ship_kind::UBoot]=shared_state->n_submarines;

    wait_setup();
}

int MainWindow::pos_pushed_button(QPushButton *pushed)
{
    int pos=0;
    for(auto& button : button_container)
    {
        if(button==pushed)
        {
            return pos;
        }
        ++pos;
    }
    return pos;
}
//draw board from _enemy vector


//void MainWindow::display_missed_shot()
//{
//    qDebug() << init_stage.pos_pushed << "missed";
//    game_button_container[init_stage.pos_pushed]->setStyleSheet("");

//    connect(destroyed_animation.get(), &QMovie::frameChanged, this, [=]{
//        game_button_container[current_game->get]->setIcon(this->destroyed_animation->currentPixmap())});

//}

//void MainWindow::display_hitted_shot()
//{
//    qDebug() << init_stage.pos_pushed << "hitted";
//    game_button_container[init_stage.pos_pushed]->setStyleSheet("");
//    buttongif[init_stage.pos_pushed] = 1;
//    //init_stage.pushed_button->setStyleSheet("background: rgb(255,0,0)");
//}

//void MainWindow::display_sunk_shot()
//{
//    //    for(auto& x: shared_state->sunk_positions)
//    //    {
//    //        connect(this->destroyed_animation.get(), &QMovie::frameChanged, this, [this, x]{
//    //            this->button_container[x]->setIcon(this->destroyed_animation->currentPixmap());
//    //        });
//    //    }
//    //    connect(this->destroyed_animation.get(), &QMovie::frameChanged, this, [this]{
//    //        init_stage.pushed_button->setIcon(this->destroyed_animation->currentPixmap());
//    //    });
//    game_button_container[init_stage.pos_pushed]->setStyleSheet("");
//    //    game_button_container[init_stage.pos_pushed]->setStyleSheet("background: rgb(0,255,0)");
//}

void MainWindow::display_defeated(){}

void MainWindow::display_won(){}
