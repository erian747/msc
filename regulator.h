
#ifndef REGULATOR_H
#define REGULATOR_H


class Regulator
{
  typedef float pid_type_t;
public:
  Regulator(pid_type_t kp=1, pid_type_t ti=0) :
    mKp(kp), mTi(ti)
  {
    mRef = 0;
    mOutput = 0;
    mI = 0;
  }
  void input(pid_type_t v)
  {
    pid_type_t e = (mRef - v);
    mI += e;
    mOutput = (mKp * e) + (mTi * mI);
    if(mOutput > mMaxOutput)
      mOutput = mMaxOutput;
    if(mOutput < mMinOutput)
      mOutput = mMinOutput;
  }
  pid_type_t output(void)
  {
    return mOutput;
  }
  void setPoint(pid_type_t sp) { mRef = sp;}
  pid_type_t getSetPoint(void) {return mRef; }
protected:

private:
  pid_type_t mKp;
  pid_type_t mTi;
  pid_type_t mRef;
  pid_type_t mOutput;
  pid_type_t mI;
  static constexpr pid_type_t mMaxOutput = 1;
  static constexpr pid_type_t mMinOutput = 0;

};

#endif
