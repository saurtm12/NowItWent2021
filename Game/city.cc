#include "city.h"

namespace Model
{

City::City(QWidget *parent):
    map_(new QGraphicsScene(parent)), pause_(false),
    isInStop(false),
    isLocked(false),
    isInBus(false)
{
    pokemons_ = readPokemonData(":/pokemonImg/Pokemon/");
}

City::~City()
{

}

void City::setBackground(QImage &basicbackground, QImage &bigbackground)
{
    //TODO: need to implement this
    map_->setBackgroundBrush(basicbackground);
}

void City::setClock(QTime clock)
{
    clock_ = clock;
    if (isInStop)
    {
        for (auto& bus : actorsMap_)
        {
            if (typeid(*(bus.first)).name() == Utils::NYSSE_TYPE)
            {
                if (bus.second->getLocation().isClose(player_->getLocation(),20))
                {
                    isInStop = false;
                    isInBus = true;
                    onBus_ = bus.second;
                    return;
                }
            }
        }
    }
    if (isInBus)
    {
        auto busLocation = onBus_->getLocation();
        player_->setTrueCoord(busLocation);
    }

}

void City::startGame()
{
    QImage backgroundImage = QImage(BACKGROUND);

    // Fix this line
    setBackground(backgroundImage,backgroundImage);
    addMainActor();
    generateBalls();
}

QGraphicsScene* City::getMap() {
    return map_;
}

QDialog* City::getPlayerBag(QWidget* parent) const
{
    return player_->getBag();
}

void City::addStop(std::shared_ptr<IStop> stop)
{
    QGraphicsPixmapItem* stopPixmap = map_->addPixmap(QPixmap::fromImage(QImage(BUS_STOP_ICON)));
    std::shared_ptr<Character> newStop = std::make_shared<Character>(stopPixmap, stop->getLocation());
    newStop->setOffset(-8, -24);
    stopsMap_.insert({ stop, newStop });
}

void City::addActor(std::shared_ptr<IActor> newactor)
{
    QString imgPath = BUS_ICON;
    if (typeid(*newactor).name() == Utils::NYSSE_TYPE)
    {
      imgPath = BUS_ICON;
    }
    if (typeid(*newactor).name() == Utils::PASSENGER_TYPE)
    {
      imgPath = PASSENGER_ICON;
    }

    QGraphicsPixmapItem* actorPixmap = map_->addPixmap(QPixmap::fromImage(QImage(imgPath)));
    std::shared_ptr<Character> actorGraphic = std::make_shared<Character>(actorPixmap, newactor->giveLocation());
    actorGraphic->setOffset(-10, -10);
    actorsMap_.insert({ newactor, actorGraphic });
    emit actorChanged(newactor, 1);
}

void City::addMainActor()
{
    QGraphicsPixmapItem* mainPixmap = map_->addPixmap(QPixmap::fromImage(QImage(STEWIE_ICON)));
    Location mainLoc;
    mainLoc.setXY(200, 200);
    player_ = std::make_shared<Player>(mainPixmap, mainLoc);
}

void City::removeActor(std::shared_ptr<IActor> actor)
{
    actorsMap_.erase(actor);
    emit actorChanged(actor, -1);
}

void City::addBall()
{
    // give consistency for the larger map.
    int x = rand() % WITDH - 352;
    int y = 558 - rand() % HEIGHT;
    Location newLoc;
    newLoc.setXY(x,y);

    QGraphicsPixmapItem* ballPixmap = map_->addPixmap(QPixmap::fromImage(QImage(BALL_ICON)));
    std::shared_ptr<Character> newBall = std::make_shared<Character>(ballPixmap,  newLoc );
    newBall->setOffset(-10, -10);
    ballsMap_.push_back(newBall);
}

void City::generateBalls()
{
    ballsMap_.clear();
    for (int i = 0; i < BALLS_PER_TURN; i++){
        addBall();
    }
}

Pokemon City::generatePokemon()
{
    // TODO: fix this random to not generate same thing
    // TODO: I get an exception when generating a pokemon.
    std::random_device rand_dev;
    std::mt19937 generator(rand_dev());
    std::uniform_int_distribution<int> distr(0, pokemons_.size());
    int idx = distr(generator);
    return pokemons_.at(idx);
}

void City::actorRemoved(std::shared_ptr<IActor> actor)
{

}

bool City::findActor(std::shared_ptr<IActor> actor) const
{
    return actorsMap_.find(actor) != actorsMap_.end();
}

void City::actorMoved(std::shared_ptr<IActor> actor)
{
    actorsMap_.at(actor)->setCoord(actor->giveLocation());
}

std::vector<std::shared_ptr<IActor> > City::getNearbyActors(Location loc) const
{
    std::vector< std::shared_ptr<IActor> > result;
    for (auto actor: actorsMap_) {
        if (actor.first->giveLocation().isClose(loc, 5)) {
            result.push_back(actor.first);
        }
    }

    return result;
}

void City::handleCollision()
{
    for (auto it = ballsMap_.begin(); it != ballsMap_.end(); ++it) {
        if (player_->getItem()->collidesWithItem((*it)->getItem())) {
            Pokemon pokemon = generatePokemon();
            player_->addPokemon(pokemon);
            ballsMap_.erase(it);
            addBall();
            emit collideBall(pokemon);
            return;
        }
    }
}

bool City::isGameOver() const
{
    return player_->getFuel() <= 0;
}

void City::keyPress(int command)
{

    switch (command) {
    case Qt::Key_W:
        if (isLocked)
        {
            break;
        }
        player_->moveDirection(0, -1);
        emit updateFuel(player_->getFuel());
        handleCollision();
        break;
    case Qt::Key_S:
        if (isLocked)
        {
            break;
        }
        player_->moveDirection(0, 1);
        emit updateFuel(player_->getFuel());
        handleCollision();
        break;
    case Qt::Key_A:
        if (isLocked)
        {
            break;
        }
        player_->moveDirection(-1, 0);
        emit updateFuel(player_->getFuel());
        handleCollision();
        break;
    case Qt::Key_D:
        if (isLocked)
        {
            break;
        }
        player_->moveDirection(1, 0);
        emit updateFuel(player_->getFuel());
        handleCollision();
        break;
    case Qt::Key_Space:
        if (!isLocked){
            isLocked = joinStop();
        }
        else{
            isLocked = false;
            isInStop = false;
            qDebug() << "player leaves bus stop";
        }
        qDebug()<< "Space pressed";
        break;
    }
    if (isGameOver())
    {
        emit gameOver();
    }
}


bool City::joinStop()
{
    auto playerLoc = player_->getLocation();
    qDebug()<< playerLoc.giveX() << playerLoc.giveY();
    for (auto& stop: stopsMap_){
        if (playerLoc.isClose(stop.second->getLocation(),20))
        {
            isInStop = true;
            player_->setTrueCoord(stop.second->getLocation());
            qDebug() << "player joins stop" <<stop.first->getName();
            return true;
        }
    }
    return false;
}

}
