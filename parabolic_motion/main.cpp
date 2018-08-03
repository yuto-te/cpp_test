/*
放物運動を解いてみる
*/

#include<iostream>
#include<cmath>

// 重力加速度
constexpr double g = 9.80665;

// 時間パラメータ
constexpr double tlim = 100.;
constexpr double dt = 0.01;


struct Condition
{
    double x;
    double y;
    double vx;
    double vy;
};

// initial condition
Condition initCondition(){
    double theta = std::atan(1) * 4. * 1. / 6.;
    double v0 = 100.;

    Condition cond{};
    cond.x = 0.;
    cond.y = 0.;
    cond.vx = v0 * std::cos(theta);
    cond.vy = v0 * std::sin(theta);
    return cond;
}

// time step
Condition step(Condition& cond){
    cond.x += cond.vx * dt;
    cond.y += cond.vy * dt;
    cond.vx = cond.vx;
    cond.vy -= g * dt;
    return cond;
}

int main()
{
    Condition c = initCondition();
    double t;

    FILE *gnuplot = popen("gnuplot -persist","w");
    fprintf(gnuplot, "set size square\n");
    fprintf(gnuplot, "set grid\n");
    fprintf(gnuplot, "plot '-' with lines\n");

    for(std::size_t i {}; t < tlim; ++i){
        // std::cout << t << " " << c.x << " " << c.y << std::endl;
        fprintf(gnuplot, "%lf, %lf\n", c.x, c.y);
        c = step(c);
        t = i * dt;
        if(c.y <= 0.){
            break;
        }
    }
}
