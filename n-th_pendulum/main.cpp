/*
n重振り子
LU分解で運動方程式を解く
RK4
*/

#include<bits/stdc++.h>
#include<Eigen/Core>
#include<Eigen/LU>

using std::sin;
using std::cos;

// おもりの数
constexpr int N = 5;

// 重力加速度
constexpr double g = 9.80665;

// 時間パラメータ
constexpr double tlim = 100;
constexpr double dt = 0.01;

// 質量と振り子の腕の長さ
using Mass = Eigen::Matrix<double, N, 1>;
using Length = Eigen::Matrix<double, N, 1>;

// 振り子の角度と角速度
struct Condition
{
    Eigen::Matrix<double, N, 1> theta;
    Eigen::Matrix<double, N, 1> dtheta;
};

Condition operator*(const double x, const Condition cond){
    Condition cond_return = {x*cond.theta, x*cond.dtheta};
    return cond_return;
}

Condition operator+(const Condition c1, const Condition c2){
    Condition c_return = {c1.theta + c2.theta, c1.dtheta + c2.dtheta};
    return c_return;
}

void initialCondition(Mass &m, Length &l, Condition &cond){
    // 振り子の質量と腕の長さを与える
    for(int i = 0; i < N; i++){
        m(i,0) = (i + 1) * 0.5;
        l(i,0) = (i + 1) * 0.5;
    };
    // 初期条件
    for(int i = 0; i < N; i++){
        cond.theta(i,0) = M_PI / 2.;
        cond.dtheta(i,0) = 0.;
    };
}

double sum(const Eigen::Matrix<double, N, 1> x, const int m, const int n){
    double total = 0.;
    for(int i = m; i < n; i++){
        total += x(i,0);
    };
    return total;
}

// 運動方程式
Condition updateCondition(Mass m, Length l, Condition cond){
    Eigen::Matrix<double, N, N> A;
    Eigen::Matrix<double, N, 1> b, x;
    for(int i = 0; i < N; i++){
        double tmp = 0.;
        for(int j = 0; j < N; j++){
            if(j < i){
                A(i, j) = sum(m, i, N)*l(j, 0)*cos(cond.theta(i, 0) - cond.theta(j, 0));
                tmp -= sum(m, i, N)*l(j, 0)*cond.dtheta(j, 0)*cond.dtheta(j, 0)*sin(cond.theta(i, 0) - cond.theta(j, 0));
            }
            else if(j == i){
                A(i, j) = sum(m, i, N)*l(i, 0);
                tmp -= sum(m, i, N)*g*sin(cond.theta(i));
            }
            else{
                A(i, j) = sum(m, j, N)*l(j, 0)*cos(cond.theta(i, 0) - cond.theta(j, 0));
                tmp -= sum(m, i, N)*l(j, 0)*cond.dtheta(i, 0)*cond.dtheta(i, 0)*sin(cond.theta(i, 0) - cond.theta(j, 0));
            }
        };
        b(i, 0) = tmp;
    };
    Eigen::FullPivLU<Eigen::Matrix<double, N, N> > LU(A);
    return Condition{
        cond.dtheta,
        LU.solve(b)
    };
}

void plot(FILE *gp, const Condition cond, const Length l){
    fprintf(gp, "plot '-' w lp lw 3\n");
    fprintf(gp, "0.0 0.0\n");
    double x = 0, y = 0;
    for(int i = 0; i < N; i++){
        x += l(i,0)*sin(cond.theta(i,0));
        y -= l(i,0)*cos(cond.theta(i,0));
        fprintf(gp, "%f %f\n", x, y);
    }
    fprintf(gp, "e\n");

}

int main(){
    Condition x;
    Mass m;
    Length l;
    Condition k1, k2, k3, k4;
    double t = 0.;

    initialCondition(m, l, x);

    FILE *gp = popen("gnuplot","w");
    fprintf(gp, "set nokey\n");
    fprintf(gp, "set size square\n");
    fprintf(gp, "set term gif animate optimize delay 0.1 size 500,500\n");
    fprintf(gp, "set output 'movie.gif'\n");
    fprintf(gp, "set xr [%f:%f]\n", -sum(l, 0, N), sum(l, 0, N));
    fprintf(gp, "set yr [%f:%f]\n", -sum(l, 0, N), sum(l, 0, N));

    for(int i = 0; t < tlim; i++){
        if(i%10 == 0){
            plot(gp, x, l);
            // std::cout << x.theta << "\n" << std::endl;
        }

        k1 = updateCondition(m, l, x);
        k2 = updateCondition(m, l, x + dt/2*k1);
        k3 = updateCondition(m, l, x + dt/2*k2);
        k4 = updateCondition(m, l, x + dt*k3);
        x = x + dt/6*(k1 + 2*k2 + 2*k3 + k4);
        t += dt;
    };
}