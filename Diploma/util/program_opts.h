#ifndef DIPLOMA_PROGRAM_OPTS_H
#define DIPLOMA_PROGRAM_OPTS_H


typedef struct short_opt_t
{
    char arg;
    int id;
} ShortOpt;

typedef struct long_opt_t
{
    char* arg;
    int id;
} LongOpt;


char* compareShortOpt(char* opt, const ShortOpt* shortOpt);

char* compareLongOpt(char* opt, const LongOpt* longOpt);

char* getShortOpt(char* opt, const ShortOpt* shortOpts, int& id);

char* getLongOpt(char* opt, const LongOpt* longOpts, int& id);

char* getOpt(char* arg, const ShortOpt* shortOpts, const LongOpt* longOpts, int& id);

#endif