#define VS                0  //デバッグ用
#define GMP               0  //現在未実装
#define WIN               1  //待機用関数、コンソール用コマンドを変更
#define ONLINE_C_CONPILER 0  // https://www.tutorialspoint.com/compile_c_online.php

#define _CRT_SECURE_NO_WARNINGS  //おせっかい防止

#include <ctype.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if WIN
#    define CLEAR "cls"
#    pragma warning(push)
#    pragma warning(disable : 5105)
#    pragma warning(disable : 4668)
#    include <windows.h>
#    pragma warning(pop)
#else
#    define CLEAR "clear"
#    include <unistd.h>
#endif

#if GMP
#    pragma warning(push)
#    pragma warning(disable : 4146)
#    include <gmp.h>
#    pragma warning(pop)
#endif

/*プロトタイプ宣言===================================================================*/
void  PrintInfo(void);                     //説明文出力
char* GetLine(int);                        // 1行読み取り（0 ;内部処理用）
bool  CheckLine(char*);                    //書式チェック（1行ポインタ）
int   CountColumn(char*);                  //列数（1行ポインタ）
int   GetDigit(long long);                 //整数桁数（整数）
void  PrintMatrix(long long**, int, int);  //整数配列出力（整数二次元配列,行数,列数）
void  NullCheck(void*);                    //メモリが足りるか（メモリを確保したポインタ）
void  Error(int);                          //エラー処理 (エラーコード)
void  Clear(void);                         //再描画
/*================================================================================*/
//再描画用//
int    nowL = 0, nowM = 0;
bool   endA = false;
char** strA;
char** strB;
//
//

int main(void) {
    PrintInfo();
    int  L = 0, M = 0, N = 0;
    bool check;

    /*行列A取得================================================================*/
    puts("1つ目の行列を入力してください");
    strA = (char**)malloc(sizeof(char*));
    // 1行目
    do {
        strA[0] = GetLine(0);
    } while (!CheckLine(strA[0]));
    M = CountColumn(strA[0]);  // M決定

    // 2行目以降
    do {
        L++;
        nowL++;
        check = false;

        char** tmp = realloc(strA, sizeof(char*) * (L + 1));
        NullCheck(tmp);

        strA = tmp;
        tmp  = NULL;

        do {
            strA[L] = GetLine(0);

            if (strA[L][0] == '.') {
                break;
            }

            check = CheckLine(strA[L]);
            if (CountColumn(strA[L]) != M) {
                check = false;
                Error(20);
            }

        } while (!check);
    } while (!(strA[L][0] == '.'));  // L決定
    endA = true;

    /*行列B取得================================================================*/
    puts("2つ目の行列を入力してください");
    strB = (char**)malloc(sizeof(char*) * M);
    // 1行目
    do {
        strB[0] = GetLine(0);
    } while (!CheckLine(strB[0]));
    N = CountColumn(strB[0]);  // N決定

    // 2行目以降
    for (nowM = 1; nowM < M; nowM++) {
        check = false;
        do {
            strB[nowM] = GetLine(0);

            check = CheckLine(strB[nowM]);
            if (CountColumn(strB[nowM]) != N) {
                check = false;
                Error(20);
            }
        } while (!check);
    }

    /*整数配列を作成================================================================*/
#if GMP  //データ型をマクロ定義で渡したほうがきれいに書けるのでは？　初期化部はどっちにしろ条件コンパイル必要だけど
    mpz_t** A = (mpz_t**)malloc(sizeof(mpz_t*) * L);  //可変長配列は使えないらしい
    for (int i = 0; i < L; i++) {
        A[i] = (mpz_t*)malloc(sizeof(mpz_t) * M);
    }

    mpz_t** B = (mpz_t**)malloc(sizeof(mpz_t*) * M);  //可変長配列は使えないらしい その2
    for (int i = 0; i < M; i++) {
        A[i] = (mpz_t*)malloc(sizeof(mpz_t) * N);
    }

    mpz_t A[L][M], B[M][N];

#else

    long long** A = (long long**)malloc(sizeof(long long*) * L);  //可変長配列は(ry その3
    NullCheck(A);

    for (int i = 0; i < L; i++) {
        A[i] = (long long*)malloc(sizeof(long long) * M);
        NullCheck(A[i]);
    }

    long long** B = (long long**)malloc(sizeof(long long*) * M);  //可変長配列(ry その4
    NullCheck(B);

    for (int i = 0; i < M; i++) {
        B[i] = (long long*)malloc(sizeof(long long) * N);
        NullCheck(B[i]);
    }

    long long** C = (long long**)malloc(sizeof(long long*) * L);  //可変長(ry その5...怒られても無視すればいけたのかも？
    NullCheck(C);
    for (int i = 0; i < L; i++) {
        C[i] = (long long*)calloc(N, sizeof(long long));
        NullCheck(C[i]);
    }

#endif

    /*ポインタから整数配列へ================================================================*/
    //入力処理と一緒にやればメモリ無駄遣いせずに済んだけど、、、書いてしまったのでこのまま
    // 行列B
    char* element = NULL;
    for (int col = 0; col < L; col++) {
        for (int row = 0; row < M; row++) {
            if (row) {
                element = strtok(NULL, ",");
            } else {
                element = strtok(strA[col], ",");
            }
            A[col][row] = atoll(element);
        }
    }
    //行列B
    for (int col = 0; col < M; col++) {
        for (int row = 0; row < N; row++) {
            if (row) {
                element = strtok(NULL, ",");
            } else {
                element = strtok(strB[col], ",");
            }
            B[col][row] = atoll(element);
        }
    }
    element = NULL;
    //要検証//
    // strtokの第一引数に"(strB[col],NULL)[row]"を渡したら怒られた
    //警告を握りつぶせばいけるのか（そもそもエラーか警告か覚えてない）
    //それとも三項演算子を使えばいけるのか、、、

    //文字配列開放
    for (int i = 0; i < L; i++) {
        free(strA[i]);
    }
    for (int i = 0; i < M; i++) {
        free(strB[i]);
    }
    free(strA);
    free(strB);
    strA = NULL;
    strB = NULL;

    /*行列の積================================================================*/
    for (int col = 0; col < L; col++) {
        for (int row = 0; row < N; row++) {
            for (int i = 0; i < M; i++) {
                C[col][row] += A[col][i] * B[i][row];
            }
        }
    }

    /*出力================================================================*/
    system(CLEAR);
    PrintInfo();
    puts(
        "<<計算結果>>\n"
        "(A) x (B) = (C)\n");
    puts(
        "(A) :\n"
        "------------------------------\n");
    PrintMatrix(A, L, M);
    puts("\n------------------------------\n");

    puts(
        "(B) :\n"
        "------------------------------\n");
    PrintMatrix(B, M, N);
    puts("\n------------------------------\n");

    puts(
        "(C) :\n"
        "------------------------------\n");
    PrintMatrix(C, L, N);
    puts("\n------------------------------\n");

#if VS
    getchar();
#endif
}  // MAIN

char* GetLine(int COUNT) {
    const int ch = getchar();
    if (ch != '\n') {
        char* line = GetLine(++COUNT);

        line[COUNT - 1] = ch;
        return line;
    } else {
        char* line  = (char*)malloc(sizeof(char) * (COUNT + 1));
        line[COUNT] = '\0';
        return line;
    }
}

bool CheckLine(char* INPUT) {
    if (strlen(INPUT) == 0) {  //未入力チェック
        Error(10);
        return false;
    }

    for (int i = 0; i < strlen(INPUT); i++) {  //入力文字チェック
        if (!isdigit(INPUT[i]) && INPUT[i] != ',' && INPUT[i] != '-') {
            Error(11);
            return false;
        }
    }

    if (strstr(INPUT, "--") != NULL) {  //入力文字チェック その2
        Error(11);
        return false;
    }

    if (strstr(INPUT, "[0-9]-") != NULL) {  //入力文字チェック その3
        Error(11);
        return false;
    }

    if (strstr(INPUT, ",-?0[1-9]") != NULL) {  //入力文字チェック その4
        Error(11);
        return false;
    }

    if (INPUT[0] == ',') {  //先頭文字チェック
        Error(12);
        return false;
    }

    if (INPUT[strlen(INPUT) - 1] == ',') {  //最終文字チェック
        Error(13);
        return false;
    }

    if (strstr(INPUT, ",,") != NULL) {  //空要素チェック
        Error(14);
        return false;
    }

    return true;
}  // CheckLine

void PrintMatrix(long long** MATRIX, int COL, int ROW) {
    //桁数
    int max = 0;

    for (int col = 0; col < COL; col++) {
        for (int row = 0; row < ROW; row++) {
            if (max < GetDigit(MATRIX[col][row])) {
                max = GetDigit(MATRIX[col][row]);
            }
        }
    }

    int length;

    for (int col = 0; col < COL; col++) {
        for (int row = 0; row < ROW; row++) {
            length = GetDigit(MATRIX[col][row]);

            if (max != length) {
                for (int i = 0; i < (max - length); i++) {
                    putchar(' ');
                }
            }
            printf("%lld", MATRIX[col][row]);
            putchar(' ');
        }
        putchar('\n');
    }
}  // PrintMatrix

int GetDigit(long long NUM) {
    int digit;
    if (NUM >= 0) {
        digit = (int)(log10(NUM) + 1);
    } else {
        digit = (int)(log10(-NUM) + 1);
        digit++;  //符号分
    }
    return digit;
}  // GetDigit

int CountColumn(char* LINE) {
    int count = 1;  // a列数
    for (int i = 0; i < strlen(LINE); i++) {
        if (LINE[i] == ',') {
            count++;
        }
    }

    return count;
}  // CountCoulm

void PrintInfo(void) {
    puts(
        "/*======================================================================"
        "===================================*/\n"
        "++行列積計算ツール++\n\n"
        "任意の行列の積を計算します。一行ずつ行列の要素をコンマ区切りで入力してください。\n"
        "最初の行の入力で1つ目の行列の列数と2つ目の行列の行数が決定されます。\n"
        "1つ目の行列を入力し終わったら「.（ピリオド）」を入力してください。\n"
        "列数が不正、数字とコンマ以外が入力された場合はその入力のみ無効になります。\n"
        "その場合、エラーメッセージが消えてから再入力できます。");
    printf("解の行列の各要素が取れる値は %lld から %lld の 整数 です。\n", LLONG_MIN, LLONG_MAX);
    puts("これを超える場合、動作は保証されません。");
    puts(
        "/*======================================================================"
        "===================================*/\n");
}  // PrintInfo

void NullCheck(void* ADDRESS) {
    if (ADDRESS == NULL) {
        Error(0);
    }
}

void Error(int ERROE_CODE) {
    Clear();
    printf("ERROR: ");
    switch (ERROE_CODE) {
        case 0:  //メモリ不足
            puts("メモリが不足しています");
            getchar();
            exit(EXIT_FAILURE);  //メモリの開放はOSがやってくれる

            /*ここでプログラム終了*/

        case 10:
            puts("未入力です");
            break;

        case 11:
            puts("数字とコンマ以外が入力されています");
            break;

        case 12:
            puts("先頭文字がコンマです");
            break;

        case 13:
            puts("最終文字がコンマです");
            break;

        case 14:
            puts("空の要素が存在しました");
            break;

        case 20:
            puts("列数が不正です");
            break;
    }
#if WIN
    Sleep(1000);
#else
    sleep(1);
#endif
    Clear();

    return;
}

void Clear(void) {
    system(CLEAR);

#if ONLINE_C_CONPILER
    puts('>');
    puts("//ホントはここで画面クリア//");
    puts('>');
#endif

    PrintInfo();
    puts("1つ目の行列を入力してください");
    if (nowL) {
        for (int l = 0; l <= nowL - 1; l++) {
            puts(strA[l]);
        }
    }
    if (endA) {
        puts(".");
        puts("2つ目の行列を入力してください");

        if (nowM) {
            for (int m = 0; m <= nowM - 1; m++) {
                puts(strB[m]);
            }
        }
    }
}
