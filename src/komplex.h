#ifndef KOMPLEXCLASS_H
#define KOMPLEXCLASS_H

namespace kplx {

typedef struct ALG_ZAHL{
    double a,b;
}ALG_ZAHL;

typedef struct EXP_ZAHL{
    double r,phi;
}EXP_ZAHL;

ALG_ZAHL ConvertToAlgForm(EXP_ZAHL z);
EXP_ZAHL ConvertToExpForm(ALG_ZAHL z);

ALG_ZAHL AddAlgForm(ALG_ZAHL z1, ALG_ZAHL z2);
ALG_ZAHL SubAlgForm(ALG_ZAHL z1, ALG_ZAHL z2);
ALG_ZAHL MulAlgForm(ALG_ZAHL z1, ALG_ZAHL z2);
ALG_ZAHL DivAlgForm(ALG_ZAHL z1, ALG_ZAHL z2);

EXP_ZAHL AddExpForm(EXP_ZAHL z1, EXP_ZAHL z2);
EXP_ZAHL SubExpForm(EXP_ZAHL z1, EXP_ZAHL z2);
EXP_ZAHL MulExpForm(EXP_ZAHL z1, EXP_ZAHL z2);
EXP_ZAHL DivExpForm(EXP_ZAHL z1, EXP_ZAHL z2);

}

#endif // KOMPLEXCLASS_H
