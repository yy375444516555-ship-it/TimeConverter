#include <Eigen>


struct transformation {
    Vector3d dR = Vector3d::Zero();
    Vector3d dV = Vector3d::Zero();
    Quarteniond Q = Quartenion::Identiry();
    Vector3d w = Vector3d::Zero(); 
    
    Vector3d operator()(const Vector3d& r) const{}
        
    std::pair<Vector3d, Vector3d> operator()(const Vector3d& r, const Vector3d& v) const{}

    transformation& operator*=(const transformation& other){}

    transformation& operator*(const transformation& other) const {
        auto cpy = *this;
        cpy *= other;
        return cpy;
    }

    void inverse(){

    }

    transformation inversed() const{
        auto cpy = *this;
        cpy.inverse();
        return cpy;
    }
};

enum class Frame {GCRS, CIRS, TIRS, ITRS};

template<typename EOP, typename TimeConverter>
class FrameConverter {
    EOP eop_;
    TimeConverter tc_;
    public:

    FrameConverter(const EOP& eop, const TimeConverter& tc) : eop_(eop), tc_(tc) {}
    //sofa и ifы    iers cinvertions 2010!!!!!
    template<Scale sc>
    transformation operator()(const Time<sc>& t, Frame to, Frame from) const {}

    const EOP& eop() const {return eop_;}
};

int main(){

    transformation T(.dR =, .dV =, .Q=, .W = );

    TimeConverter tc;
    EOP eop;

    FrameConverter fc(eop, tc);

    const auto tr1 = fc(TT, Frame::GCRS, Frame::ITRS);

    Vector3d r_ITRS;
    Vector3d r_GCRS = tr1(r_ITRS);

}

//sofa cookbook