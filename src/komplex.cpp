#include "komplex.h"
#include <math.h>

namespace kplx {

    ALG_ZAHL ConvertToAlgForm(EXP_ZAHL z)
    {
        ALG_ZAHL z2;

        z2.a = z.r*cos(z.phi);
        z2.b = z.r*sin(z.phi);

        return z2;
    }

    EXP_ZAHL ConvertToExpForm(ALG_ZAHL z)
    {
        EXP_ZAHL z2;

        z2.r=sqrt(pow(z.a,2)+pow(z.b,2));
        z2.phi=atan2(z.b,z.a);

        return z2;
    }

    ALG_ZAHL AddAlgForm(ALG_ZAHL z1, ALG_ZAHL z2)
    {
        ALG_ZAHL z;
        z.a = z1.a + z2.a;
        z.b = z1.b + z2.b;
        return z;
    }

    ALG_ZAHL SubAlgForm(ALG_ZAHL z1, ALG_ZAHL z2)
    {
        ALG_ZAHL z;
        z.a = z1.a - z2.a;
        z.b = z1.b - z2.b;
        return z;
    }

    ALG_ZAHL MulAlgForm(ALG_ZAHL z1, ALG_ZAHL z2)
    {
        EXP_ZAHL ze,ze1,ze2;
        ze1 = ConvertToExpForm(z1);
        ze2 = ConvertToExpForm(z2);
        ze = MulExpForm(ze1,ze2);
        return ConvertToAlgForm(ze);
    }

    ALG_ZAHL DivAlgForm(ALG_ZAHL z1, ALG_ZAHL z2)
    {
        EXP_ZAHL ze,ze1,ze2;
        ze1 = ConvertToExpForm(z1);
        ze2 = ConvertToExpForm(z2);
        ze = DivExpForm(ze1,ze2);
        return ConvertToAlgForm(ze);
    }

    EXP_ZAHL AddExpForm(EXP_ZAHL z1, EXP_ZAHL z2)
    {
        ALG_ZAHL za,za1,za2;
        za1 = ConvertToAlgForm(z1);
        za2 = ConvertToAlgForm(z2);
        za = AddAlgForm(za1,za2);
        return ConvertToExpForm(za);
    }

    EXP_ZAHL SubExpForm(EXP_ZAHL z1, EXP_ZAHL z2)
    {
        ALG_ZAHL za,za1,za2;
        za1 = ConvertToAlgForm(z1);
        za2 = ConvertToAlgForm(z2);
        za = SubAlgForm(za1,za2);
        return ConvertToExpForm(za);
    }

    EXP_ZAHL MulExpForm(EXP_ZAHL z1, EXP_ZAHL z2)
    {
        EXP_ZAHL z;
        z.r = z1.r * z2.r;
        z.phi = z1.phi + z2.phi;
        return z;
    }

    EXP_ZAHL DivExpForm(EXP_ZAHL z1, EXP_ZAHL z2)
    {
        EXP_ZAHL z;
        z.r = z1.r / z2.r;
        z.phi = z1.phi - z2.phi;
        return z;
    }
}
