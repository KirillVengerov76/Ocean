//Kirill Vengerov 9"V"

#include "TXLib.h"

enum Objtype { GRaSS, SToNE, YEsU, PLAyER };

class Cworld
{
protected:
    int x;//coordinate x
    int y;//coordinate y
    int l;//direction
    int r;//intaraction radius
    int xl;//x length
    int yl;//y length
public:
    int hungr;//hunger
protected:
    HDC creature;//picture
    Cworld() : x(0), y(0), l(0), r(0), xl(0), yl(0), hungr(0), creature(nullptr) {}
    Cworld(int xw, int yw, int lw, int rw, int xlw, int ylw, int hungrw, HDC creature1) : x(xw), y(yw), l(lw), r(rw), xl(xlw), yl(ylw), hungr(hungrw), creature(creature1) {}
public:
    virtual ~Cworld() {}
    void update()
    {
        change();
        show();
    }
    virtual void change() = 0;
    virtual void show() const = 0;
    virtual bool isDead() const { return 0; }
    bool hasCollisionWith( Cworld* pOther ) const;
    virtual void collideWith( Cworld* pOther ) {}
};

class Cplayer: public Cworld
{
public:
    Cplayer(int x0, int y0, int l0, int r0, int xl0, int yl0, int hungr0, HDC creature2) : Cworld( x0, y0, l0, r0, xl0, yl0, hungr0, creature2 ) {}
    virtual void change() override
    {
        hungr++;
        if(GetAsyncKeyState(VK_UP))
        {
            y-=5;
        }
        if(GetAsyncKeyState(VK_DOWN))
        {
            y+=5;
        }
        if(x <= -30)
        {
            x = 740;
        }
        x -= 5;
        if (hungr < -1)
        {
            y = -10000;
        }
        if (hungr == -1)
        {
            y = 10;
        }
    }
    virtual void show() const override
    {
        Win32::TransparentBlt (txDC(), x, y, xl, yl, creature,0,0,512,512,RGB(255,255,255));
    }
};

class Cyellow_submarine: public Cworld
{
public:
    Cyellow_submarine(int x0, int y0, int l0, int r0, int xl0, int yl0, int hungr0,HDC creature2) : Cworld( x0, y0, l0, r0, xl0, yl0, hungr0, creature2 ) {}
    virtual void change() override
    {
        if(x > 576)
        {
            l = -1;
        }
        if(x < 0)
        {
            l = 1;
        }
        x+=6*l;
        hungr++;
        if(hungr >= 100)
        {
            y+=5;
        }
        else
        {
            y-=5;
        }
        if(y < 150)
        {
            y+=5;
        }
        if(y > 233)
        {
            y-=5;
        }
        if (hungr < -1)
        {
            y = -10000;
        }
        if (hungr == -1)
        {
            y = 150;
        }
    }
    virtual void show() const override
    {
        Win32::TransparentBlt (txDC(), x, y, xl, yl, creature,0,0,1678,992,RGB(0,32,128));
    }
    virtual void collideWith( Cworld* pOther ) override;

};

class Cgrass: public Cworld
{
public:
    Cgrass(int x0, int y0, int l0, int r0, int xl0, int yl0, int hungr0,HDC creature2) : Cworld( x0, y0, l0, r0, xl0, yl0, hungr0, creature2 ) {}
    virtual void change() override
    {
        if(y > 263)
        {
            y--;
        }
        int i;
        i = (516 - y)/36;
        for(int j = 0; j <= i; j++)
            Win32::TransparentBlt (txDC(), 360, 516-(36*j), 72, 36, creature,0,0,1200,720,RGB(0,0,0));
    }
    virtual void show() const override
    {
        Win32::TransparentBlt (txDC(), x, y, xl, yl, creature,0,0,1200,720,RGB(0,0,0));
    }
    virtual void collideWith( Cworld* pyellow_submarine ) override;
};

class Cstone: public Cworld
{
public:
    Cstone& operator= (const Cstone &s) {
        x = s.x;
        y = s.y;
        l = s.l;
        r = s.r;
        creature = s.creature;
        return *this;
    }
    Cstone() : Cworld() {}
    Cstone(int x0, int y0, int l0, int r0, int xl0, int yl0, int hungr0,HDC creature2) : Cworld( x0, y0, l0, r0, xl0, yl0, hungr0, creature2 ) {}
    virtual void change() override {}
    virtual void show() const override
    {
        Win32::TransparentBlt (txDC(), x, y, 72, 36, creature,0,0,1024,512,RGB(255,255,255));
    }
    void set(int X, int Y, int L, int R, int XL, int YL, int HUNGR, HDC h)
    {
        x = X;
        y = Y;
        l = L;
        r = R;
        xl = XL;
        yl = YL;
        hungr = HUNGR;
        creature = h;
    }
};

bool Cworld::hasCollisionWith( Cworld* pOther ) const
{
    int x1 = pOther->x;
    int y1 = pOther->y;
    int xl1 = pOther->xl;
    int yl1 = pOther->yl;
    int hungr1 = pOther->hungr;
    if (hungr1 + hungr >= 100)
    {
        double distance = hypot( (x + (xl/2)) - (x1 + (xl1/2)), (y + (yl/2)) - (y1 + (yl1/2)) );
        return (distance < r + pOther->r);
    }
    return false;
}

void Cyellow_submarine::collideWith( Cworld* pOther )
{
    auto pplayer = dynamic_cast <Cplayer*> (pOther);
    if( pplayer )
    {
        hungr = -100;
        pOther->hungr = -100;
    }
}

void Cgrass::collideWith( Cworld* pOther )
{
    auto pgrass = dynamic_cast <Cyellow_submarine*> (pOther);
    if( pgrass )
    {
        y += 36;
        pOther->hungr = 0;
    }
}

class COcean
{
private:
    std::vector <Cworld*> pObje;
public:
    void addObj( Objtype type, int x0, int y0, int l0, int r0, int xl0, int yl0, int hungr0,HDC creature0 );
    void update();
    void checkCollisions() const;
    void removeDead();
};

void COcean::addObj( Objtype type, int x0, int y0, int l0, int r0, int xl0, int yl0, int hungr0,HDC creature0 )
{
    Cworld* pNewObj = nullptr;
    if(type == SToNE)
    {
        pNewObj = new Cstone(x0, y0, l0, r0, xl0, yl0, hungr0, creature0);
    }
    else if(type == GRaSS)
    {
        pNewObj = new Cgrass(x0, y0, l0, r0, xl0, yl0, hungr0, creature0);
    }
    else if(type == YEsU)
    {
        pNewObj = new Cyellow_submarine(x0, y0, l0, r0, xl0, yl0, hungr0, creature0);
    }
    else if(type == PLAyER)
    {
        pNewObj = new Cplayer(x0, y0, l0, r0, xl0, yl0, hungr0, creature0);
    }
    if( pNewObj )
    {
        pObje.push_back( pNewObj );
    }
}

void COcean::update()
{
    for( auto pObj: pObje )
    {
        pObj->update();
    }
    checkCollisions();
    removeDead();
}

void COcean::checkCollisions() const
{
    for( unsigned int i = 0; i < pObje.size(); i++ )
    {
        for( unsigned int j = i+1; j < pObje.size(); j++ )
        {
          if( pObje[i]->hasCollisionWith(pObje[j]) )
            {
                pObje[i]->collideWith( pObje[j] );
                pObje[j]->collideWith( pObje[i] );
            }
        }
    }
}

void COcean::removeDead()
{
    for( int i = pObje.size()-1; i >= 0; i-- )
    {
        if( pObje[i]->isDead() )
        {
            delete pObje[i];
            pObje.erase( pObje.begin()+i );
        }
    }
}

int main()
{
    HDC GRASS = txLoadImage ("grass.bmp");
    HDC STONE = txLoadImage ("stone.bmp");
    HDC YESUB = txLoadImage ("yellow_submarine.bmp");
    HDC TORP  = txLoadImage ("torpedo.bmp");
    HDC fon   = txLoadImage ("water.bmp");
    int number_ston = 10;
    COcean oc;
    for(int i = 0; i < number_ston; i++)
    {
        oc.addObj ( SToNE,i*72, 444,0,0,0,0,0,STONE);
    }
    oc.addObj ( PLAyER,10,10,10,0,51,51,1000,TORP);//доделать
    oc.addObj ( GRaSS,360,300,400,0,72,36,0,GRASS);
    oc.addObj ( YEsU,40,150,1,75,144,85,1,YESUB);
    txCreateWindow(720,480);
    txBegin();
    txClear();
    while(!GetAsyncKeyState(VK_ESCAPE))
    {
        Win32::TransparentBlt (txDC(), 0, 0, 720, 480, fon,0,0,853,480,RGB(0,0,0));
        oc.update();
        txSleep(100);
    }
    txEnd();
    return 0;
}
