#include <iostream>
#include <string.h>

#include "defs.h"

int ThreeFoldRep(const S_BOARD *pos) {
    int i = 0, r = 0;
    for(i = 0; i < pos->hisPly; ++i) {
        if(pos->history[i].posKey == pos->posKey) {
            r++;
        }
    }

    return r;
}

int DrawMaterial(const S_BOARD *pos) {
    if(pos->pceNum[Constants::Enums::wP] || pos->pceNum[Constants::Enums::bP]) return FALSE;
    if(pos->pceNum[Constants::Enums::wQ] || pos->pceNum[Constants::Enums::bQ] || pos->pceNum[Constants::Enums::wR] || pos->pceNum[Constants::Enums::bR]) return FALSE;
    if(pos->pceNum[Constants::Enums::wB] > 1 || pos->pceNum[Constants::Enums::bB] > 1) return FALSE;
    if(pos->pceNum[Constants::Enums::wN] > 1 || pos->pceNum[Constants::Enums::bN] > 1) return FALSE;
    if(pos->pceNum[Constants::Enums::wN] && pos->pceNum[Constants::Enums::wB]) return FALSE;
    if(pos->pceNum[Constants::Enums::bN] && pos->pceNum[Constants::Enums::bB]) return FALSE;

    return TRUE;
}

int checkresult(S_BOARD *pos) {
    S_MOVELIST list[1];
    int MoveNum = 0;
    int found = 0;
    int InCheck;

    if(pos->fiftyMove > 100) {
        printf("1/2-1/2 (fifty move rule (claimed by Vice))\n"); return TRUE;
    }

    if(ThreeFoldRep(pos)) {
        printf("1/2-1/2 (3-fold repetition (claimed by Vice))\n"); return TRUE;
    }

    if(DrawMaterial(pos) == TRUE) {
        printf("1/2-1/2 (insufficient material (claimed by Vice))\n"); return TRUE;
    }

    GenerateAllMoves(pos, list);
    for(MoveNum = 0; MoveNum < list->count; ++MoveNum) {
        if(!MakeMove(pos, list->moves[MoveNum].move)) {
            continue;
        }
        found++;
        TakeMove(pos);
        break;
    }

    if(found != 0) return FALSE;

    InCheck = SqAttacked(pos->KingSq[pos->side], pos->side ^ 1, pos);

    if(InCheck == TRUE) {
        if(pos->side == Constants::Enums::WHITE) {
            printf("0-1 (black mates \n"); return TRUE;
        } else {
            printf("0-1 (white mates \n"); return TRUE;
        }
    } else {
        printf("1/2-1/2 (stalemate \n"); return TRUE;
    }

    return FALSE;
}


void XBoard_Loop(S_BOARD *pos, S_SEARCHINFO *info) {
    int depth = -1, movestogo[2] = {30,30 }, movetime = -1;
    int time = -1, inc = 0;
    int engineSide = Constants::Enums::BOTH;
    int timeLeft;
    int sec;
    int mps;
    int move = NOMOVE;
    char inBuf[80], command[80];
    int MB;
    TimeUtility tm;

    info->GAME_MODE = Constants::Enums::XBOARDMODE;
    info->POST_THINKING = TRUE;
    setbuf(stdin, NULL);
    setbuf(stdout, NULL);
//    PrintOptions(); // HACK

    engineSide = Constants::Enums::BLACK;
    ParseFen(START_FEN, pos);
    depth = -1;
    time = -1;

    while(TRUE) {

        fflush(stdout);

        if(pos->side == engineSide && checkresult(pos) == FALSE) {
            info->starttime = tm.GetTimeMs();
            info->depth = depth;

            if(time != -1) {
                info->timeset = TRUE;
                time /= movestogo[pos->side];
                time -= 50;
                info->stoptime = info->starttime + time + inc;
            }

            if(depth == -1 || depth > Constants::MAXDEPTH) {
                info->depth = Constants::MAXDEPTH;
            }

            printf("time:%d start:%d stop:%d depth:%d timeset:%d movestogo:%d mps:%d\n",
                   time, info->starttime, info->stoptime, info->depth, info->timeset, movestogo[pos->side], mps);
            SearchPosition(pos, info);

            if(mps != 0) {
                movestogo[pos->side ^ 1]--;
                if(movestogo[pos->side ^ 1] < 1) {
                    movestogo[pos->side ^ 1] = mps;
                }
            }
        }

        fflush(stdout);

        memset(&inBuf[0], 0, sizeof(inBuf));
        fflush(stdout);
        if (!fgets(inBuf, 80, stdin))
            continue;

        sscanf(inBuf, "%s", command);

        printf("command seen:%s\n",inBuf);

        if(!strcmp(command, "quit")) {
            info->quit = TRUE;
            break;
        }

        if(!strcmp(command, "force")) {
            engineSide = Constants::Enums::BOTH;
            continue;
        }

        if(!strcmp(command, "protover")){
//            PrintOptions();
            continue;
        }

        if(!strcmp(command, "sd")) {
            sscanf(inBuf, "sd %d", &depth);
            printf("DEBUG depth:%d\n",depth);
            continue;
        }

        if(!strcmp(command, "st")) {
            sscanf(inBuf, "st %d", &movetime);
            printf("DEBUG movetime:%d\n",movetime);
            continue;
        }

        if(!strcmp(command, "time")) {
            sscanf(inBuf, "time %d", &time);
            time *= 10;
            printf("DEBUG time:%d\n",time);
            continue;
        }

        if(!strcmp(command, "polykey")) {
            PrintBoard(pos);
            GetBookMove(pos);
            continue;
        }

        if(!strcmp(command, "memory")) {
            sscanf(inBuf, "memory %d", &MB);
            if(MB < 4) MB = 4;
            if(MB > Constants::MAX_HASH) MB = Constants::MAX_HASH;
            printf("Set Hash to %d MB\n",MB);
            InitHashTable(pos->HashTable, MB);
            continue;
        }

        if(!strcmp(command, "level")) {
            sec = 0;
            movetime = -1;
            if( sscanf(inBuf, "level %d %d %d", &mps, &timeLeft, &inc) != 3) {
                sscanf(inBuf, "level %d %d:%d %d", &mps, &timeLeft, &sec, &inc);
                printf("DEBUG level with :\n");
            }	else {
                printf("DEBUG level without :\n");
            }
            timeLeft *= 60000;
            timeLeft += sec * 1000;
            movestogo[0] = movestogo[1] = 30;
            if(mps != 0) {
                movestogo[0] = movestogo[1] = mps;
            }
            time = -1;
            printf("DEBUG level timeLeft:%d movesToGo:%d inc:%d mps%d\n",timeLeft,movestogo[0],inc,mps);
            continue;
        }

        if(!strcmp(command, "ping")) {
            printf("pong%s\n", inBuf+4);
            continue;
        }

        if(!strcmp(command, "new")) {
            ClearHashTable(pos->HashTable);
            engineSide = Constants::Enums::BLACK;
            ParseFen(START_FEN, pos);
            depth = -1;
            time = -1;
            continue;
        }

        if(!strcmp(command, "setboard")){
            engineSide = Constants::Enums::BOTH;
            ParseFen(inBuf+9, pos);
            continue;
        }

        if(!strcmp(command, "go")) {
            engineSide = pos->side;
            continue;
        }

        if(!strcmp(command, "usermove")){
            movestogo[pos->side]--;
            move = ParseMove(inBuf+9, pos);
            if(move == NOMOVE) continue;
            MakeMove(pos, move);
            pos->ply=0;
        }
    }
}


void Console_Loop(S_BOARD *pos, S_SEARCHINFO *info) {
    int depth = Constants::MAXDEPTH, movetime = 3000;
    int engineSide = Constants::Enums::BOTH;
    int move = NOMOVE;
    char inBuf[80], command[80];
     TimeUtility tm;


    printf("you are now in  Console Mode!\n");
    printf("Type help for commands\n\n");

    info->GAME_MODE = Constants::Enums::CONSOLEMODE;
    info->POST_THINKING = TRUE;
    engineSide = Constants::Enums::BLACK;
    ParseFen(START_FEN, pos);

    setbuf(stdin, NULL);
    setbuf(stdout, NULL);

    while(TRUE) {

        fflush(stdout);

        if(pos->side == engineSide && checkresult(pos) == FALSE) {
            info->starttime =tm.GetTimeMs();
            info->depth = depth;

            if(movetime != 0) {
                info->timeset = TRUE;
                info->stoptime = info->starttime + movetime;
            }

            SearchPosition(pos, info);
        }

        printf("\nVice > ");

        fflush(stdout);

        memset(&inBuf[0], 0, sizeof(inBuf));
        fflush(stdout);
        if (!fgets(inBuf, 80, stdin))
            continue;

        sscanf(inBuf, "%s", command);

        if(!strcmp(command, "help")) {
            printf("Commands:\n");
            printf("quit - quit game\n");
            printf("force - computer will not think\n");
            printf("print - show board\n");
            printf("post - show thinking\n");
            printf("nopost - do not show thinking\n");
            printf("new - start new game\n");
            printf("go - set computer thinking\n");
            printf("depth x - set depth to x\n");
            printf("time x - set thinking time to x seconds (depth still applies if set)\n");
            printf("view - show current depth and movetime settings\n");
            printf("setboard x - set position to fen x\n");
            printf("** note ** - to reset time and depth, set to 0\n");
            printf("enter moves using b7b8q notation\n\n\n");
            continue;
        }

        if(!strcmp(command, "mirror")) {
            engineSide = Constants::Enums::BOTH;
            MirrorEvalTest(pos);
            continue;
        }

        if(!strcmp(command, "eval")) {
            PrintBoard(pos);
            printf("Eval:%d",EvalPosition(pos));
            MirrorBoard(pos);
            PrintBoard(pos);
            printf("Eval:%d",EvalPosition(pos));
            continue;
        }

        if(!strcmp(command, "setboard")){
            engineSide = Constants::Enums::BOTH;
            ParseFen(inBuf+9, pos);
            continue;
        }

        if(!strcmp(command, "quit")) {
            info->quit = TRUE;
            break;
        }

        if(!strcmp(command, "post")) {
            info->POST_THINKING = TRUE;
            continue;
        }

        if(!strcmp(command, "print")) {
            PrintBoard(pos);
            continue;
        }

        if(!strcmp(command, "nopost")) {
            info->POST_THINKING = FALSE;
            continue;
        }

        if(!strcmp(command, "force")) {
            engineSide = Constants::Enums::BOTH;
            continue;
        }

        if(!strcmp(command, "view")) {
            if(depth == Constants::MAXDEPTH) printf("depth not set ");
            else printf("depth %d",depth);

            if(movetime != 0) printf(" movetime %ds\n",movetime/1000);
            else printf(" movetime not set\n");

            continue;
        }

        if(!strcmp(command, "depth")) {
            sscanf(inBuf, "depth %d", &depth);
            if(depth==0) depth = Constants::MAXDEPTH;
            continue;
        }

        if(!strcmp(command, "time")) {
            sscanf(inBuf, "time %d", &movetime);
            movetime *= 1000;
            continue;
        }

        if(!strcmp(command, "new")) {
            ClearHashTable(pos->HashTable);
            engineSide =Constants::Enums::BLACK;
            ParseFen(START_FEN, pos);
            continue;
        }

        if(!strcmp(command, "go")) {
            engineSide = pos->side;
            continue;
        }

        move = ParseMove(inBuf, pos);
        if(move == NOMOVE) {
            printf("Command unknown:%s\n",inBuf);
            continue;
        }
        MakeMove(pos, move);
        pos->ply=0;
    }
}
