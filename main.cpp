#define DEBUG

#include <QCoreApplication>
#include <string>
#include <QRandomGenerator>
#include <QDebug>
#include <QtGlobal>

#include "defs.h"
#include "headrs/bitboard.h"
#include "headrs/init.h"
#include "headrs/sqattack.h"
#include "headrs/board.h"
//#include "headrs/evaluate.h"






int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
     AllInit();


    S_BOARD pos[1];
    S_SEARCHINFO info[1];
    char line[256];
    int ArgNum = 0;

    pos->HashTable->pTable = NULL;



    InitHashTable(pos->HashTable, 64);
    memset(pos->searchKillers, 0, 2 * Constants::MAXDEPTH * sizeof(int));

    setbuf(stdin, NULL);
    setbuf(stdout, NULL);

    for(ArgNum = 0; ArgNum < argc; ++ArgNum) {
        if (std::string(argv[ArgNum]) == "NoBook") {
          UseBook = false;
        }

//        printf("Welcome to Vice! Type 'vice' for console mode...\n");
        std::cout<< "Welcome to chess engine, console mode.." << std::endl;

    }



    while (TRUE) {
        memset(&line[0], 0, sizeof(line));

        fflush(stdout);
        if (!fgets(line, 256, stdin))
          continue;
        if (line[0] == '\n')
          continue;
        if (!strncmp(line, "uci",3)) {
          Uci_Loop(pos, info);
          if(info->quit == TRUE) break;
          continue;
        } else if (!strncmp(line, "xboard",6))	{
          XBoard_Loop(pos, info);
          if(info->quit == TRUE) break;
          continue;
        } else if (!strncmp(line, "setcon",4))	{
          Console_Loop(pos, info);
          if(info->quit == TRUE) break;
          continue;
        } else if(!strncmp(line, "quit",4))	{
          break;
        }
    }

    free(pos->HashTable->pTable);
    CleanPolyBook();
    return a.exec();

}
