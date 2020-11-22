#ifndef BUS_HH
#define BUS_HH

#include "../Course/CourseLib/actors/nysse.hh"
#include "../Course/CourseLib/offlinereader.hh"
#include "character.hh"
#include "Utils/helpers.hh"

namespace Model
{

class Bus : public CourseSide::Nysse, public Model::Character
{
public:
    Bus(std::shared_ptr<CourseSide::BusData> busData);

    QTime getNextSchedule();
    QTime advanceTime();

    void setCoord(Interface::Location loc);

private:
    std::list<QTime> schedule_;
    // start time iterator
    std::list<QTime>::iterator timeIter_;
    QTime time_;
};

}

#endif // BUS_HH
