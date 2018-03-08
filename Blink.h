#ifndef __BLINK_H__
#define __BLINK_H__

class Blink
{
public:
    Blink(int pin);
    virtual ~Blink();

    void on(unsigned int milliseconds);
    void off(unsigned int milliseconds);
private:
    int mPin;
};


#endif /*__BLINK_H__*/

