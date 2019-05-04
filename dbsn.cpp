#include "dbsn.h"

//-- создать пустой массив целых чисел
int getLongArray( int count ) {
    int*  pl = new int[count];
    for( int j = 0; j<count; j++ )
        *(pl+j) = 0;
    return  (int) pl;
};

//-- сосчитать длину строки
int getLen( char* pc) {
    if( !pc ) return - BAD_PTR;
    int     len=0;
    while(*pc++)  len++;
    return  len;
}

//-- сосчитать число подстрок в строке
int substCount( char* pc) {
    if( !pc ) return - BAD_PTR;
    int     res = 1;
    for( ; *pc; pc++ )
        if( *pc =='|')  res++;
    return res;
};

//-- вытащить подстроку с заданным номером из строки в буфер pcres
int getSubst(char* pc, char* pcres, int res_size, int nom_sub) {
    if( !pc || !pcres ) return - BAD_PTR;
    int     len = getLen( pc),
            subst_count = substCount(pc);
    char    *pc_src = pc, *pc_dst = pcres;
    if(pc[len-1]=='\n')  pc[len-1]=0; //-- корректиируем последний конец строки
    *pcres = 0;
    if( nom_sub <0) nom_sub += subst_count; //-- отриц считаем с конца
    if( (nom_sub <0)||(nom_sub >= subst_count))
        return  - BAD_NUMBER;
    //-- есть всего одна подстрока
    if(( nom_sub == 0)&& (subst_count==1))
    {   do {
            if( pc_dst - pcres >= res_size - 1) {
                *pc_dst = 0;  return  - BUFFER_SMALL;
            };
            *pc_dst++ = *pc_src++;
        }while( *pc_src);
        *pc_dst = 0;      //-- 17 aug 2013
        return pc_dst - pcres - 1;
    }
    //-- не одна подстрока и выбрана последняя подстрока
    if (nom_sub == subst_count-1)
    {   pc_src = pc + len;
        while(*pc_src != '|') pc_src--;
        for( pc_src++; *pc_src;){
            if( pc_dst - pcres >= res_size - 1) {
                *pc_dst = 0;  return  - BUFFER_SMALL;
            };
            *pc_dst++ = *pc_src++;
        }
        *pc_dst = 0;        //-- 12 mar 2013
        return pc_dst - pcres - 1;
    }
    //-- не последняя подстрока
    for( ;nom_sub; pc_src++ )
    {   if( *pc_src != '|' ) continue;
        nom_sub--;
    }
    //-- нашли начало подстроки
    while((*pc_src)&&(*pc_src != '|')){
        if( pc_dst - pcres >= res_size - 1) {
            *pc_dst = 0;  return  - BUFFER_SMALL;
        };
        *pc_dst++ = *pc_src++;
    }
    *pc_dst = 0;
    return pc_dst - pcres - 1;
};

//==  получить в буфер начальную строку текста( заголовок)
//--  возвращает длину полученной строки
int getTextHead(char* pc, char* pcres, int res_size )
{
    if( !pc || !pcres ) return - BAD_PTR;
    char*  pc_dst = pcres;
    while((*pc)&&(*pc != '\n')&&(*pc != '\r')){
        if( pc_dst - pcres >= res_size - 1) {
            *pc_dst = 0;  return  - BUFFER_SMALL;
        };
        *pc_dst++ = *pc++;
    }
    *pc_dst = 0;
    return  pc_dst - pcres - 1;
}

//==  получить остаток текста без начальной строки( заголовка )
int getTextBody(char* pc, char* pcres, int res_size)
{
    if( !pc || !pcres ) return - BAD_PTR;
    while((*pc)&&(*pc != '\n'))
        pc++;
    if( *pc == '\n')
        pc++;
    char*  pc_dst = pcres;
    while(*pc){
        if( pc_dst - pcres >= res_size - 1) {
            *pc_dst = 0;  return  - BUFFER_SMALL;
        };
        *pc_dst++ = *pc++;
    }
    *pc_dst = 0;
    return  pc_dst - pcres - 1;
}

//-- запись целого числа в бинарный файл
int  fwriteInt( int fh, int val)
{
    FILE *  pFile = (FILE *)fh;
    char*   pc = (char*)&val;
    fwrite(pc,1,4,pFile);
    return 0;
}

//-- чтение целого числа из бинарного файла
int  freadInt( int fh )
{
    FILE *  pFile = (FILE *)fh;
    int     val;
    char*   pc = (char*)&val;
    fread(pc,1,4,pFile);
    return val;
}
//== сосчитать число строк в тексте ========================
int  countTextSt(char* pc)
{
    if( !pc ) return - BAD_PTR;
    if( *pc == 0) return 0;
    int     res = 1;
    for( ; *pc; pc++ )
    	if( *pc == '\n') res++;
    //-- учитываем последнюю строку
    if( *(pc-1) == '\n') res--;
    return res;
}

//== получить строку с заданным номером из текста ==========
//-- счет ведется от 0
int  getTextSt(char*  pc, char* pcres, int res_size,  int st_nom)
{
    if( !pc || !pcres ) return - BAD_PTR;
    int  st_count = countTextSt( pc );
    if(( st_nom < 0)||(st_nom >= st_count)) {
        *pcres = 0;    return - BAD_NUMBER;
    }
    int   st_beg;
    char  *pc_dst = pcres;
    //-- ищем начало нужной строки
    for( ; st_nom > 0; pc++ )
        if( *pc == '\n' ) st_nom--;
    //-- копируем найденную строку
    for( ; (*pc)&&( *pc != '\n');  ){
        if( pc_dst - pcres >= res_size - 1) {
            *pc_dst = 0;  return  - BUFFER_SMALL;
        };
        *pc_dst++ = *pc++;
    }
    *pc_dst = 0;
    return  pc_dst - pcres - 1;
}
//-------------------------------------------------
//-- новая функция
//-- пока без проверки
char* trim( char* pc, char* pcres)
{   //-- ищем начало
    while( (*pc)&&((*pc == ' ')||(*pc =='\t')))
        pc++;
    if( !*pc )  //-- вся строка белая или пустая
    {   *pcres = 0;  return pcres;
    }
    //-- ищем конец непустой строки
    char*  pc_end = pc;
    while( *pc_end)  pc_end++;
    //-- идем назад от конца
    pc_end--;
    while( (*pc_end == ' ')||(*pc_end == '\t'))
        pc_end--;
    //--копируем кусок строки
    char*  pc_dst = pcres;
    for( pc_end++;  pc != pc_end; )
        *pc_dst++ = *pc++;
    *pc_dst =0;
    return pcres;
}
//----------------------------------------------------------
//== общие операции с базой ====
//== *** beg *** ==
//== создать пустую таблицу фрагментов по имени файла
//-- возвращает 0 при успехе (число записей в таблице )
//-- или отрицательный код ошибки
int createDBSN( char*  fil_name)
{
    char*   pc_buff = new char[ getLen( fil_name )+1];
    int     err_cod = 0;
    fil_name = trim(fil_name, pc_buff);
    if(getLen(fil_name) == 0){  //-- блокируем пробельное имя
        err_cod = BAD_TABLE_NAME;
        return -err_cod;
    }
    char*  fil_dbs = new char[ getLen( fil_name )+ 5];
    strcpy( fil_dbs, fil_name);
    strcat( fil_dbs, ".dbs");
    int     fh = (int)fopen(fil_dbs, "w");
    if(!fh) {               //-- не смогли создать файл dbs
        err_cod = DBS_NOT_EXIST;
        return -err_cod;
    }
    fclose((FILE*)fh);
    strcpy( fil_dbs, fil_name);
    strcat( fil_dbs, ".dbn");
    fh = (int)  fopen(fil_dbs, "wb");
    if(!fh){               //-- не смогли создать файл dbn
        err_cod = DBN_NOT_EXIST;
        return -err_cod;
    }
    fclose((FILE*)fh);
    return 0;
}
//----------------------------------------------------------

//== проверить существование файлов DBSN
//-- возвращает 1 при успехе
//-- или отрицательный код ошибки
int  existDBSN( char* fil_name ) {
    int     err_cod = 0;
    char*  pc_buff = new char[ getLen( fil_name )+1];
    fil_name = trim(fil_name, pc_buff);
    if( getLen(fil_name) == 0){  //-- блокируем пробельное имя
        err_cod = BAD_TABLE_NAME;
        return -err_cod;
    }
    char*  fil_dbs = new char[ getLen( fil_name )+ 5];
    strcpy( fil_dbs, fil_name);
    strcat( fil_dbs, ".dbs");
    int     fhs=(int)fopen(fil_dbs,"rb");
    if(!fhs){                   //-- не смогли открыть файл dbs
        err_cod = DBS_NOT_EXIST;
        return -err_cod;
    }
    fclose((FILE*)fhs);
    strcpy( fil_dbs, fil_name);
    strcat( fil_dbs, ".dbn");
    int     fhn=(int)fopen(fil_dbs,"rb");
    if(!fhn){                   //-- не смогли открыть файл dbn
        err_cod = DBN_NOT_EXIST;
        return -err_cod;
    }
    fclose((FILE*)fhn);
    return 1;
};
//----------------------------------------------------------

//== получить все содержимое DBS файла
//-- возвращает длину файла при успехе
//-- или отрицательный код ошибки
int getAllDBS( char*  fil_name, char* &text ) {
    int     res = existDBSN( fil_name);
    if( res <= 0){   //-- таблица не существует
        *text = 0;  return  res;
    }
        //-- открываем текстовый файл
    char*  fil_dbs = new char[ getLen( fil_name )+ 5];
    strcpy( fil_dbs, fil_name);
    strcat( fil_dbs, ".dbs");
    int     fhl = (int) fopen(fil_dbs,"rb");
    fseek((FILE*)fhl,0,2);
    int  file_size = ftell((FILE*)fhl);
        //-- создаем буфер в памяти
    text  =  new char[ file_size +1 ];
    void *pv =  (void*)text;
    if( !pv ){  //-- не хватает свободной памяти
        res = BUFFER_SMALL;  return -res;
    }
        //-- считываем в него весь файл
    fread( pv, 1,1,(FILE*)fhl);
    fclose((FILE*)fhl);
    return file_size;
}
//----------------------------------------------------------

//== открыть таблицу фрагментов DBSN
//--  возвращает обработчик таблицы при успехе
//--  (адрес массива параметров )
//--  или отрицательный код ошибки
int openDBSNA( char* fil_name ) {
    int     res = existDBSN( fil_name );
    if( res < 0 ) return  res;

    char*  fil_dbs = new char[ getLen( fil_name )+ 5];
    strcpy( fil_dbs, fil_name);
    strcat( fil_dbs, ".dbn");
    int     fhn = (int)fopen(fil_dbs, "rb+");
    int     dbhadr =  getLongArray( 4 );
    int*    dbh = (int*) dbhadr;
        //-- заполняем обработчик таблицы
    dbh[0] = fhn;       //-- обработчик числового файла
    strcpy( fil_dbs, fil_name);
    strcat( fil_dbs, ".dbs");
    int     fhs = (int)fopen(fil_dbs, "rb+");
    dbh[1] = fhs;       //-- обработчик текстового файла
    int     rec_nom = 0;
    dbh[2] = rec_nom;   //-- номер текущей зприси
    dbh[3] = 0;         //-- код ошибки
    return  dbhadr;
}
//----------------------------------------------------------

//== Закрыть таблицу фрагментов DBSN
//-- возвращает 0 при выполнении для обнуления dbhadr
//-- или отрицвтельный код фатальной ошибки
//-- стандартный способ использования
//      dbhadr  =  closeDBSN( dbhadr );
//-- после этого можно повторно использовать  dbhadr
int  closeDBSN( int &dbhadr ) {
    int     err_cod = 0;
    if( !dbhadr ){  //-- не задан обработчик
        err_cod = BAD_TABLE_HANDLE;  return -err_cod;
    }
    int*  dbh = (int*) dbhadr;
    err_cod = dbh[3];
    if( err_cod >= FATAL_ERROR)  return -err_cod;
    int     fhn = dbh[0];
    fflush( (FILE*)fhn );
    fclose((FILE*)fhn);
    int     fhs = dbh[1];
    fflush( (FILE*)fhs );
    fclose((FILE*)fhs);
    delete[] dbh;
    dbhadr = 0;
    return 0;
}
//----------------------------------------------------------

//== Удалить все фрагменты таблицы DBSN
//--  просто делает их длины отрицательными
//--  возвращает количество записей в таблице
//-- (совпадает с номером последней записи)
//--  или отрицвтельный код фатальной ошибки
int  delAllFragm(  int dbhadr ) {
    int     err_cod = 0;
    if( !dbhadr ){  //-- не задан обработчик
        err_cod = BAD_TABLE_HANDLE;  return -err_cod;
    }
    int*  dbh = (int*) dbhadr;
    err_cod = dbh[3];
    if( err_cod >= FATAL_ERROR )  return -err_cod;

    dbh[3] = 0; //-- стираем предупреждения
    //-- определяем количество записей
    int     fhn = dbh[0];
    fseek( (FILE*)fhn,0,2);
    int     rec_count = ftell( (FILE*)fhn ) /8;
    int     rec_size = 0;
    //-- в цикле удаляем все записи
    for( int  rec_nom = 1; rec_nom <= rec_count; rec_nom++)
    {	//-- считываем координаты строчного фрагмента
        fseek( (FILE*)fhn, (rec_nom-1)*8 + 4, 0);
        rec_size = freadInt( fhn );
        if(rec_size > 0)
        {	rec_size = -rec_size;
            fseek( (FILE*)fhn, (rec_nom-1)*8 + 4, 0);
            fwriteInt( fhn, rec_size );
        }
    }
    dbh[2] = rec_count;
    return rec_count;
};
//----------------------------------------------------------

//== Полностью уничтожить  таблицу DBSN
//-- возвращает 1 при успехе
//-- или отрицательный код ошибки
int  eraseDBSN( char*  fil_name ) {
    int     err_cod = 0;
    char*  pc_buff = new char[ getLen( fil_name )+1];
    fil_name = trim(fil_name, pc_buff);
    if( getLen(fil_name) == 0){  //-- блокируем пробельное имя
        err_cod = BAD_TABLE_NAME;
        return -err_cod;
    }
    char*  fil_dbs = new char[ getLen( fil_name )+ 5];
    strcpy( fil_dbs, fil_name);
    strcat( fil_dbs, ".dbs");
    remove(fil_dbs);
    strcpy( fil_dbs, fil_name);
    strcat( fil_dbs, ".dbn");
    remove(fil_dbs);
    return 1;
}
//----------------------------------------------------------

//==  Переименовать таблицу DBSN в новое имя
//--  возвращает 1 при успехе
//--  или отрицательный код ошибки
int  renameDBSN( char* old_fil, char* new_fil ) {
    int     err_cod = 0;
    if(!existDBSN(old_fil)) {
        err_cod = BAD_TABLE_NAME;
        return -err_cod;
    }
    char*  oldfil_dbs = new char[ getLen( old_fil )+ 5];
    strcpy( oldfil_dbs, old_fil);
    strcat( oldfil_dbs, ".dbs");
    char*  newfil_dbs = new char[ getLen( new_fil )+ 5];
    strcpy( newfil_dbs, new_fil );
    strcat( newfil_dbs, ".dbs" );
    rename( oldfil_dbs, newfil_dbs);

    strcpy( oldfil_dbs, old_fil);
    strcat( oldfil_dbs, ".dbn");
    strcpy( newfil_dbs, new_fil );
    strcat( newfil_dbs, ".dbn" );
    rename( oldfil_dbs, newfil_dbs);

    return 1;
};
//----------------------------------------------------------

//== проверить на конец файла таблицу DBSN
//--  возвращает 1 если записей нет или вышли за границу
//--  или отрицательный код ошибки
int  isEndDBSN( int dbhadr ) {
    int     err_cod = 0;
    if( !dbhadr ){  //-- не задан обработчик
        err_cod = BAD_TABLE_HANDLE;  return -err_cod;
    }

    int*    dbh = (int*) dbhadr;
    err_cod = dbh[3];
    if( err_cod >= FATAL_ERROR )  return -err_cod;

   	if( dbh[3] & OUT_OF_RANGE )
		return 1;
	if( dbh[3] & DBN_IS_EMPTY )
		return 1;
	return 0;
}
//----------------------------------------------------------
//== *** end *** ==
//==========================================================

//== работа с блоками фрагментов  ==========================
//-- номерация блоков от 1 до  countNblocks включительно

//----------------------------------------------------------
//== 07 apr 2013 == работа с бинарными фрагментами =========
//== 20 янв 2014 ====
//-- каждый бинарный фрагмент состоит из бмнарных подстрок =
//-- Binsubst  вмда:   ДДДДббббббббббб...ббб[000]   где
//--    ДДДД - четырехбайтная длина строки(смысловой части),
//--    бббббб...бб - ее содержимое (произвольные байты)
//--    если длина подстроки не кратна 4 она дополняется 0-ми в конце
//--    len -  длина в байтах(для бинарного фрагмента всегда кратна 4
//--    size - размер в словах ( size = len /4)
//==========================================================

//-- сосчитать длину информации бинарной подстроки в байтах
int  binsubstLenSh( char* pc ) {
    return  *(int*) pc;
}
//----------------------------------------------------------

//-- вычислить место занимаемое бинарной подстрокой в словах
int  binsubstSizeSh( char* pc ) {
    int     loc_size = 4 + binsubstLenSh(  pc );
    loc_size = ( loc_size + 3 ) /4;  //-- число занятых слов
    return  loc_size;
}
//----------------------------------------------------------

//-- сосчитать число подстрок в бинарной строке
//-- внутренняя функция без проверки правильности параметров
int binsubstCountIn( char* pcFragm, int fragmLen) {
    int     count = 0, offset = 0;
    while( offset < fragmLen ) {
        count++;
        offset +=  binsubstSizeSh( pcFragm + offset )*4;
    }
    return count;
};
//----------------------------------------------------------
//-- сосчитать число подстрок в бинарной строке
int binsubstCount( char* pcFragm, int fragmLen) {
    if (( !pcFragm )||( !fragmLen))  return - BAD_PTR ;
    return binsubstCountIn( pcFragm, fragmLen);
};
//----------------------------------------------------------

//== Получить смещение бинарной подстроки в байтах
//-- pcFragm - бинарный фрагмент с подстроками
//-- fragmLen - его длина для контроля ошибки
//-- nom_sub - номер требуемой бинарной подстроки
//-- при ошибке возвращает -BAD_NUMBER
//-- внутренняя функция без проверки правильности параметров
int   binsubstOffsetIn(char* pcFragm, int fragmLen, long nom_sub) {
    int     count = 0, offset = 0;
    while(( offset < fragmLen )&&( count < nom_sub )) {
        count++;   offset +=  binsubstSizeSh( pcFragm + offset )*4;
    }
    return  offset;  //-- реальное смещение подстроки
}

//-- внешний вариант функции  --
int   binsubstOffset(char* pcFragm, int fragmLen, long nom_sub) {
    if (( !pcFragm )||( !fragmLen))  return - BAD_PTR;
    if( nom_sub > binsubstCountIn( pcFragm, fragmLen) )  return -BAD_NUMBER;
    return  binsubstOffsetIn( pcFragm, fragmLen, nom_sub);
}
//----------------------------------------------------------

//== получить указатель на бинарную подстроку с заданным номером из строки
//-- возвращает 0 для слишком больших номеров или пустого фрагмента
//-- внутренняя функция без проверки правильности параметров
char* getBinsubstPtIn(char* pcFragm, int fragmLen, long nom_sub) {
    int     offset = binsubstOffsetIn( pcFragm, fragmLen, nom_sub);
    return  pcFragm + offset;
}

//-- внешний вариант функции  --
char* getBinsubstPt(char* pcFragm, int fragmLen, long nom_sub) {
    if (( !pcFragm )||( !fragmLen))  return  0;
    if( nom_sub > binsubstCountIn( pcFragm, fragmLen) )  return 0;
    return  getBinsubstPtIn( pcFragm, fragmLen, nom_sub);
}
//----------------------------------------------------------

//== сосчитать длину подстроки по ее номеру ================
//-- внутренняя функция без проверки правильности параметров
int  binsubstLenIn( char* pcFragm, int fragmLen, long nom_sub ) {
    return  binsubstLenSh( getBinsubstPtIn( pcFragm, fragmLen, nom_sub ) );
}

//-- внешний вариант функции  --
int  binsubstLen( char* pcFragm, int fragmLen, long nom_sub ) {
    if (( !pcFragm )||( !fragmLen))  return - BAD_PTR;
    if( nom_sub > binsubstCountIn( pcFragm, fragmLen) )  return -BAD_NUMBER;
    return  binsubstLenIn(  pcFragm, fragmLen, nom_sub );
}
//----------------------------------------------------------

//== сосчитать размер подстроки в словах по ее номеру ======
//-- внутренняя функция без проверки правильности параметров
int  binsubstSizeIn( char* pcFragm, int fragmLen, long nom_sub ) {
    return  binsubstSizeSh( getBinsubstPtIn( pcFragm, fragmLen, nom_sub ) );
}

//-- внешний вариант функции  --
int  binsubstSize( char* pcFragm, int fragmLen, long nom_sub ) {
    if (( !pcFragm )||( !fragmLen))  return - BAD_PTR;
    if( nom_sub > binsubstCountIn( pcFragm, fragmLen) )  return -BAD_NUMBER;
    return  binsubstSizeIn(  pcFragm, fragmLen, nom_sub );
}
//----------------------------------------------------------

//-- скопировать бинарную подстроку с заданным номером из строки в буфер
//-- внутренний вариант без проверки размера буфера
//-- возвращает число скопированных байт (длину бинарной подстроки + 4)
int  getBinsubstIn(char* pcFragm, int fragmLen, char* pcBuf, long nom_sub)
{   //-- ищем начало бинарной подстроки и ее длину
    char*  pcBinSubst = getBinsubstPtIn( pcFragm, fragmLen, nom_sub);
    int     loc_len = binsubstSizeSh( pcBinSubst )*4;
    for( int  loc_nom = 0; loc_nom < loc_len; loc_nom++ ) {
        *pcBuf++ = *pcBinSubst++;
    }
    return  loc_len;
};

//== внешний вариант функции с проверками
int  getBinsubst(char* pcFragm, int fragmLen, char* pcBuf, int buf_size, long nom_sub){
    if (( !pcFragm )||( !fragmLen))  return - BAD_PTR;
    if( nom_sub > binsubstCountIn( pcFragm, fragmLen) )  return -BAD_NUMBER;
    if( buf_size < binsubstSizeIn( pcFragm, fragmLen, nom_sub )*4 ) return  -BUFFER_SMALL;
    return  getBinsubstIn( pcFragm, fragmLen, pcBuf, nom_sub);
}

//----------------------------------------------------------

//== добпвить к фрагменту подстроку ========================
//-- Имеется буфер pcBuf размеровм BufLen
//-- в него копируется фрагмент и к концу дозаписывается бинарная подстрока pcBinsubst
//-- возвращается новая длина фрагмента
//-- пои ошибке возвращает отрицательный код
//-- внутренний вариант
int addBinsubstIn( char* pcFragm, int fragmLen, char* pcBuf, int bufLen, char* pcBinsubst ) {
    int  byte_nom = 0;
    for( byte_nom = 0; byte_nom < fragmLen; byte_nom++ )
        *pcBuf++ = *pcFragm++;
    int     count = binsubstSizeSh( pcBinsubst )*4;
    for( byte_nom = 0; byte_nom < count; byte_nom++ )
        *pcBuf++ = *pcBinsubst++;
    return  count + fragmLen;
}

//== внешний вариант =======================================
int addBinsubst( char* pcFragm, int fragmLen, char* pcBuf, int bufLen,  char* pcBinSubst ) {
    if (( !pcFragm )||( !pcBinSubst))  return - BAD_PTR;
    if( bufLen-fragmLen < binsubstSizeSh( pcBinSubst )*4 ) return  -BUFFER_SMALL;
    return  addBinsubstIn( pcFragm, fragmLen, pcBuf, bufLen, pcBinSubst );
}
//----------------------------------------------------------

//== удалить из фрагмента подстроку ========================
//-- Имеется буфер pcFragm
//-- из его удаляется бинарная подстрока с заданным номером
//-- возвращается новая длина фрагмента
//-- внутренний вариант без проверки на ошибку
int cutBinsubstIn( char* pcFragm, int fragmLen, long nom_sub ) {
    int  offset = binsubstOffsetIn( pcFragm, fragmLen, nom_sub);
    int  loc_len = binsubstSizeSh( pcFragm + offset )*4;
    for( int  byte_nom = offset; byte_nom < fragmLen - loc_len; byte_nom++ )
        pcFragm[ byte_nom ] = pcFragm[ byte_nom + loc_len ] ;
    return  fragmLen - loc_len;
}

//== внешний вариант c обработкой ошибок
//-- пои ошибке возвращает  отрицательный код
int cutBinsubst( char* pcFragm, int fragmLen, long nom_sub ) {
    if (( !pcFragm )||( !fragmLen))  return - BAD_PTR;
    if( nom_sub > binsubstCountIn( pcFragm, fragmLen) )  return -BAD_NUMBER;
    return  cutBinsubstIn(  pcFragm,  fragmLen, nom_sub );
}
//----------------------------------------------------------

//== очистить подстроку  в фрагменте ========================
//-- Имеется буфер pcFragm
//-- бинарная подстрока с заданным номером в нем обнуляется
//-- возвращается новая длина фрагмента
//-- внутренний вариант без проверки на ошибку
int clearBinsubstIn( char* pcFragm, int fragmLen, long nom_sub ) {
    int  offset = binsubstOffsetIn( pcFragm, fragmLen, nom_sub);
    int  loc_len = binsubstSizeSh( pcFragm + offset )*4;
    int*    pl = (int*) (pcFragm+offset);   *pl = 0;  //-- нулевая длина
    loc_len -= 4;
    for( int  byte_nom = offset+4; byte_nom < fragmLen - loc_len; byte_nom++ )
        pcFragm[ byte_nom ] = pcFragm[ byte_nom + loc_len ] ;
    return  fragmLen - loc_len;
}

//== внешний вариант c обработкой ошибок
//-- пои ошибке возвращает  отрицательный код
int clearBinsubst( char* pcFragm, int fragmLen, long nom_sub ) {
    if (( !pcFragm )||( !fragmLen))  return - BAD_PTR;
    if( nom_sub > binsubstCountIn( pcFragm, fragmLen) )  return -BAD_NUMBER;
    return  clearBinsubstIn(  pcFragm,  fragmLen, nom_sub );
}
//----------------------------------------------------------

//== вставить подстроку перед заданной во фрагмент
//-- Имеется буфер pcBuf размеровм bufLen
//-- в него копируется фрагмент со вставлеггой в него новой подстрокой
//-- возвращается новая длина фрагмента

//-- внутренний вариант без проверки на ошибку
int insBinsubstIn( char* pcFragm, int fragmLen, char* pcBuf, int bufLen, long nom_sub, char* pcBinsubst ){
    int     loc_len = binsubstSizeSh( pcBinsubst )*4;
    int     byte_nom = 0;
        //-- копируем в буфер начальную часть фрагмента
    int     offset = binsubstOffsetIn( pcFragm, fragmLen, nom_sub);
    for( byte_nom = 0; byte_nom < offset; byte_nom++ ) {
            *pcBuf++ = *pcFragm++;
    }
        //-- вставляем в буфер новую подстроку
    for( byte_nom = 0; byte_nom < loc_len; byte_nom++ ) {
            *pcBuf++ = *pcBinsubst++;
    }
        //-- дозаписываем остаток фрагмента
    for( byte_nom = offset; byte_nom < fragmLen; byte_nom++ ) {
            *pcBuf++ = *pcFragm++;
    }
        //-- возвращаем новую длину фрагмента
    return  loc_len + fragmLen;
}

//== внешний вариант c обработкой ошибок
//-- пои ошибке возвращает  отрицательный код
int insBinsubst( char* pcFragm, int fragmLen, char* pcBuf, int bufLen, long nom_sub, char* pcBinsubst  ){
    if (( !pcFragm )||( !pcBinsubst))  return - BAD_PTR;
    if( bufLen-fragmLen < binsubstSizeSh( pcBinsubst )*4 ) return  -BUFFER_SMALL;
    if( nom_sub >= binsubstCountIn( pcFragm, fragmLen) )  return -BAD_NUMBER;
    return  insBinsubstIn(  pcFragm, fragmLen, pcBuf, bufLen, nom_sub, pcBinsubst );
}

//----------------------------------------------------------

//== заменить подстроку фрагмента на новую
//-- Имеется буфер pcBuf размеровм bufLen
//-- в него копируется фрагмент с замененной на новое значение подстрокой
//-- возвращается новая длина фрагмента

//-- внутренний вариант без проверки на ошибку
int setBinsubstIn( char* pcFragm, int fragmLen, char* pcBuf, int bufLen, long nom_sub, char* pcBinsubst ){
    int     loc_len = binsubstSizeSh( pcBinsubst )*4;
    int     old_len = binsubstSizeIn( pcFragm, fragmLen, nom_sub )*4;
    int     byte_nom = 0;
        //-- копируем в буфер начальную часть фрагмента
    int     offset = binsubstOffsetIn( pcFragm, fragmLen, nom_sub);
    for( byte_nom = 0; byte_nom < offset; byte_nom++ ) {
            *pcBuf++ = *pcFragm++;
    }
        //-- вставляем в буфер новую подстроку
    for( byte_nom = 0; byte_nom < loc_len; byte_nom++ ) {
            *pcBuf++ = *pcBinsubst++;
    }
        //-- дозаписываем остаток фрагмента
    for( byte_nom = offset + old_len, pcFragm += old_len; byte_nom < fragmLen; byte_nom++ ) {
            *pcBuf++ = *pcFragm++;
    }
        //-- возвращаем новую длину фрагмента
    return  loc_len - old_len + fragmLen;
}

//== внешний вариант c обработкой ошибок
//-- пои ошибке возвращает  отрицательный код
int setBinsubst( char* pcFragm, int fragmLen, char* pcBuf, int bufLen, long nom_sub, char* pcBinsubst){
    if (( !pcFragm )||( !pcBinsubst))  return - BAD_PTR;
    if( bufLen-fragmLen < binsubstSizeSh( pcBinsubst )*4 ) return  -BUFFER_SMALL;
    if( nom_sub > binsubstCountIn( pcFragm, fragmLen) )  return -BAD_NUMBER;
    return  setBinsubstIn( pcFragm, fragmLen, pcBuf, bufLen, nom_sub,  pcBinsubst );
}
//----------------------------------------------------------

//== 04 aug 2013 ==== active Records  ======================

//-- сравнить 2 строки с учетом подстрок и игнорирование регистра
//-- регистр игнорирует, возвращает
//-- 1  если *pc > *pc1
//-- 0  если *pc == *pc1
//-- -1 если *pc < *pc1
//-- при сравнении границу подстрок "|" считает меньше любого симв
//-- останавливается по 0 любой из строк
long    compSpec( char* pc, char* pc1){
    unsigned  char  bt, bt1;
    while( 1 ){
        bt = *pc++;  bt1 = *pc1++;
        //-- отрабатываем концы строк
        if(( bt == 0 )&&( bt1 == 0 ))  return 0;
        if( bt == 0 )   return -1;
        if( bt1 == 0 )  return  1;
        //-- отрабатываем концы подстрок
        if(( bt == '|' )&&( bt1 == '|' ))  continue;
        if( bt == '|' )   return -1;
        if( bt1 == '|' )  return  1;
        //-- равниваем без учета регистра
        bt = toupper(bt);  bt1 = toupper(bt1);
        if( bt == bt1 )  continue;
        if( bt > bt1 )   return  1;
        if( bt < bt1 )   return -1;
    }
};
//----------------------------------------------------------


//== 09 aug 2013 ===========================================
//-- запись 2х целых чисел в бинарный файл
int* writeBinRec( int  dbhadr, int* plbuf)
{
    int*    dbh = (int*) dbhadr;
    if( dbh[3] > FATAL_ERROR )  return 0;
    int     fhn = dbh[0];
    int     rec_nom = dbh[2];
    FILE *  pFile = (FILE *)fhn;
    char*   pc = (char*)plbuf;
    fseek(  pFile, (  rec_nom-1)*8, 0);
    fwrite(pc,1,8,pFile);
    return plbuf;
}
//----------------------------------------------------------

//-- чтение 2х целых чисел из бинарного файла
int* readBinRec( int  dbhadr, int* plbuf )
{
    int*    dbh = (int*) dbhadr;
    if( dbh[3] > FATAL_ERROR )  return 0;
    int     fhn = dbh[0];
    int     rec_nom = dbh[2];
    FILE *  pFile = (FILE *)fhn;
    fseek(  pFile, (  rec_nom-1)*8, 0);
    char*   pc = (char*)plbuf;
    fread(pc,1,8,pFile);
    return  plbuf;
}
//----------------------------------------------------------

//== обмен местами двух фрагментов (в бьинарном файле)
//--  меняет местами текущую и заданную запись в бинпрном файле
//--  возвращает номер записи dest_nom или 0 при ошибке
//--  база устанвлена в dest_nom
int  xchgFragm( int  dbhadr, int  dest_nom ) {
        //-- выполняем общие проверки
    if( !dbhadr )  return 0;
    int     cur_nom = getNom( dbhadr );
    if( cur_nom == dest_nom )  return cur_nom;
        //-- считываем 8 байтов исходной записи
    int     cur_rec[2],
            dest_rec[2],
            tmp;
    readBinRec( dbhadr, cur_rec );
        //-- проверяем адрес назначения
    if( dest_nom < 1) return 0;
    int     rec_count = countFragmA( dbhadr );
    if( dest_nom > rec_count ) return 0;
        //-- считываем 8 байтов следующей записи
    setNom( dbhadr, dest_nom );  readBinRec( dbhadr, dest_rec );
        //-- меняем местами данные записей
    for(int j = 0; j<2; j++) {
        tmp = cur_rec[j];  cur_rec[j] = dest_rec[j];  dest_rec[j] = tmp;
    }
        //-- записываем результаты в бинарный файл
    setNom( dbhadr, cur_nom );  writeBinRec( dbhadr, cur_rec );
    setNom( dbhadr, dest_nom );  writeBinRec( dbhadr, dest_rec );
    return   dest_nom;
};
//----------------------------------------------------------

//== сбосить на диск все буфера таблицы
int    flushDBSN( int dbhadr ){
    int*    dbh = (int*) dbhadr;
    int     fhn =   dbh[0];
    if( !fhn ) return 0;
    fflush( (FILE*)fhn );
    fhn =   dbh[1];
    if( !fhn ) return 0;
    fflush( (FILE*)fhn );
    return 1;
};
//----------------------------------------------------------

//== Полностью вырезать данные фрагмента  из dbn
//-- возвращает номер следующего фрагмента или последнего
//-- или 0 при ошибке
int    cutFragm(  int dbhadr ){
    int*    dbh = (int*) dbhadr;
    int     fhn =   dbh[0];
    if( !fhn ) return 0;
    if( dbh[3] > FATAL_ERROR )  return 0;
    dbh[3] = 0; //-- стираем предупреждения

    int     fragm_nom = dbh[2];  //-- тек номер фрагмента
    int     file_off = (fragm_nom-1)*8; //-- смещение удаляемого фрагмента
    int     clast_nom = file_off/ 4096; //-- номер кластера с ним
    fseek((FILE*)fhn,0,2);
    int     file_len =  ftell( (FILE*)fhn );

    if( !file_len )
        dbh[3] = DBN_IS_EMPTY;
    if( file_len < file_off ) return 0;
    int     fragm_count = file_len /8;

    int     clast_count = (file_len + 4095)/4096;   //-- всего кластеров в бинарном файле
    char    buff[ 8192 ];      //-- буфер для 2х кластеров
    int     cur_offset = file_off - clast_nom * 4096; //-- смещ в кластере
    //-- считываем 2 фрагмента в буфер
    fseek((FILE*)fhn, clast_nom*4096 ,0);
    int     read_len =  fread((void*) buff, 1, 8192, (FILE*)fhn);
    int     nom = 0;
    //-- сдвигаем на 8 влево хвост
    for( nom = cur_offset; nom < read_len -8; nom++)
        buff[ nom ] = buff[ nom +8 ];
    if( clast_nom >= clast_count - 2 ) {  //-- короткий файл
        fseek((FILE*)fhn, clast_nom*4096 ,0);
        fwrite((void*) buff, read_len - 8, 1, (FILE*)fhn);
    }else{  //-- надо досдвигать хвост не влезший в буфер
        //-- записываем первый кластер
        fseek((FILE*)fhn, clast_nom*4096 ,0);
        fwrite((void*) buff, 4096, 1, (FILE*)fhn);
        clast_nom +=1;
        while( clast_nom < clast_count-1 ){
            //-- сдвигаем на его место второй
            for( nom = 0; nom < 4096; nom++)
            buff[ nom ] = buff[ nom +4096 ];
            //-- дописываем в конец буфера следующий кластер
            fseek((FILE*)fhn, (clast_nom+1)*4096 ,0);
            read_len =  fread((void*) (buff+4096), 1, 4096, (FILE*)fhn);
            //-- сдвигаем второй кластер влево на 8 байт
            for( nom = 4096; nom < 8192; nom++)
                buff[ nom-8 ] = buff[ nom ];
            //-- сбрасываем первую половину буфера на диск
            fseek((FILE*)fhn, clast_nom*4096 ,0);
            fwrite((void*) buff, 4096, 1, (FILE*)fhn);
            clast_nom++;
        }
        //-- дописывакм последний буфер в конце
        if(read_len > 8){
            fseek((FILE*)fhn, clast_nom*4096 ,0);
            fwrite((void*) buff, read_len-8, 1, (FILE*)fhn);
        }
    }
    if( fragm_nom >= fragm_count ) fragm_nom --;
    dbh[2] = fragm_nom;
    ftruncate(fileno( (FILE*)fhn ), file_len - 8);
    return  fragm_nom;
};
//----------------------------------------------------------


//== Переместить текущий фрагмента в заданное место dbn
//--  возвращает dest_nom
//--  или 0 при ошибке
int    moveFragm(  int dbhadr, int  dest_nom ) {
    //-- стандартная проверка на ошибку
    int*    dbh = (int*) dbhadr;
    int     fhn =   dbh[0];
    if( !fhn )  return 0;
    if( dbh[3] > FATAL_ERROR )  return 0;
    dbh[3] = 0; //-- стираем предупреждения

    int     curr_nom = dbh[2];  //-- тек номер фрагмента
    if( dest_nom == curr_nom )  //-- перемещение не нужно
        return  curr_nom;

    fseek((FILE*)fhn,0,2);
    int     file_len =  ftell( (FILE*)fhn );
    if( !file_len ){ //-- защита на пустой файл
        dbh[3] = DBN_IS_EMPTY;
        return 0;
    }

    int     fragm_count = file_len /8;
    if( ( dest_nom < 1 )||( dest_nom > fragm_count) )
        return  0;  //-- защита на входной параметр

    //-- Подготовительные операции
    int     curr_off   = (curr_nom-1)*8; //-- смещение исходного фрагмента
    int     curr_clast = curr_off/ 4096; //-- номер кластера с ним
    int     loc_curroff    = curr_off % 4096; //-- локальное смещение исх фрагм
    int     dest_off   = (dest_nom-1)*8; //-- смещение целевой позиции
    int     dest_clast = dest_off/ 4096; //-- номер кластера с ней
    int     loc_destoff    = dest_off % 4096; //-- локальное смещение целевой позиции

    char    buff[ 8192 ];      //-- буфер для 2х кластеров
    char    curr_rec[8];      //-- буфер для текущей записи
    int     byte_nom;
    int     begin_pos;
    int     write_len = 0;
    int     clast_nom;

    //-- запоминаем сдвигаемую запись
    fseek((FILE*)fhn, curr_clast*4096 ,0);
    int    read_len =  fread((void*) buff, 1, 4096, (FILE*)fhn);
    for( byte_nom = 0; byte_nom < 8; byte_nom++)
        curr_rec[byte_nom] = buff[ loc_curroff + byte_nom ];

    //-- выполнение сдвига
    if( dest_nom > curr_nom ) { //-- выполнить сдвиг фрагмента вправо
        if(  curr_clast == dest_clast ){ //--в одном кластере
            //-- сдвигаем промежуток влаво
            for( byte_nom = loc_curroff; byte_nom < loc_destoff + 8; byte_nom++)
                buff[byte_nom] = buff[ byte_nom + 8 ];
            //-- записываем исходную запись в новое место
            for( byte_nom = 0; byte_nom < 8; byte_nom++)
                buff[ byte_nom + loc_destoff ] = curr_rec[byte_nom];
            //-- сохраняем кластер на диске
            fseek((FILE*)fhn, curr_clast*4096 ,0);
            write_len =  fwrite((void*) buff, 1, read_len, (FILE*)fhn);
        } else { //-- в разных кластерах
            //-- сдвигаем промежуток влево
            for( byte_nom = loc_curroff; byte_nom < 4096; byte_nom++)
                buff[byte_nom] = buff[ byte_nom + 8 ];
            while ( 1 ){ //-- сдвигаем влево все промежуточные кластера
                write_len = read_len;
                //-- считываем предидущий кластер
                curr_clast++;
                fseek((FILE*)fhn, curr_clast*4096 ,0);
                read_len =  fread((void*) (buff + 4096 ), 1, 4096, (FILE*)fhn);
                if(  curr_clast != dest_clast) { //-- не крайний кластер
                    //-- сдвигаем его влево
                    for( byte_nom = 4096; byte_nom < 8192; byte_nom++)
                        buff[byte_nom - 8] = buff[ byte_nom ];
                    //-- записываем первую половину буфера
                    fseek((FILE*)fhn, ( curr_clast + 1)*4096 ,0);
                    write_len =  fwrite((void*) buff, 1, 4096, (FILE*)fhn);
                } else { //-- добрались до крайнего кластера
                    //-- сдвигаем его влево
                    for( byte_nom = 4096; byte_nom < 4096 +loc_destoff + 8; byte_nom++)
                        buff[byte_nom - 8] = buff[ byte_nom ];
                    //-- записываем исходную запись в новое место
                    for( byte_nom = 0; byte_nom < 8; byte_nom++)
                        buff[ byte_nom + 4096 + loc_destoff ] = curr_rec[byte_nom];
                    //-- записываем весь буфер на диск
                    fseek((FILE*)fhn, curr_clast*4096 ,0);
                    write_len =  fwrite((void*) buff, 1, 4096 + read_len, (FILE*)fhn);
                    break;
                }
            }
        }
    }  else {//-- выполнить сдвиг фрагмента влево
        if(  curr_clast == dest_clast ) { //--в одном кластере
            //-- сдвигаем промежуток вправо
            for( byte_nom = loc_curroff - 1; byte_nom >= loc_destoff; byte_nom--)
                buff[byte_nom + 8] = buff[ byte_nom ];
            //-- записываем исходную запись в новое место
            for( byte_nom = 0; byte_nom < 8; byte_nom++)
                buff[ byte_nom + loc_destoff ] = curr_rec[byte_nom];
            //-- сохраняем кластер на диске
            fseek((FILE*)fhn, curr_clast*4096 ,0);
            write_len =  fwrite((void*) buff, 1, read_len, (FILE*)fhn);
        } else { //-- в разных кластерах
            //-- сдвигаем промежуток вправо
            for( byte_nom = loc_curroff - 1; byte_nom >= 0; byte_nom--)
                buff[byte_nom + 8] = buff[ byte_nom ];
            while ( 1 ){ //-- сдвигаем вправо все промежуточные кластера
                //-- переносим прочитанное во вторую половину буфера
                for( byte_nom = 0; byte_nom < 4096; byte_nom++)
                buff[byte_nom + 4096] = buff[ byte_nom ];
                write_len = read_len;
                //-- считываем предидущий кластер
                curr_clast--;
                fseek((FILE*)fhn, curr_clast*4096 ,0);
                read_len =  fread((void*) buff, 1, 4096, (FILE*)fhn);

                if(  curr_clast != dest_clast) { //-- не крайний кластер
                    //-- сдвигаем его вправо
                    for( byte_nom = 4095; byte_nom >= 0; byte_nom--)
                        buff[byte_nom + 8] = buff[ byte_nom ];
                    //-- записываем вторую половину буфера
                    fseek((FILE*)fhn, ( curr_clast + 1)*4096 ,0);
                    write_len =  fwrite((void*) buff, 1, write_len, (FILE*)fhn);
                } else { //-- добрались до крайнего кластера
                    //-- сдвигаем его вправо
                    for( byte_nom = 4095; byte_nom >= loc_destoff; byte_nom--)
                        buff[byte_nom + 8] = buff[ byte_nom ];
                    //-- записываем исходную запись в новое место
                    for( byte_nom = 0; byte_nom < 8; byte_nom++)
                        buff[ byte_nom + loc_destoff ] = curr_rec[byte_nom];
                    //-- записываем весь буфер на диск
                    fseek((FILE*)fhn, curr_clast*4096 ,0);
                    write_len =  fwrite((void*) buff, 1, write_len + read_len, (FILE*)fhn);
                    break;
                }
            }
        }
    }
    dbh[2] = dest_nom;
    return  dest_nom;
};
//----------------------------------------------------------

//==  Вставить запись в базу фрагментов DBSN в текущую позицию
int    insFragm(  int dbhadr,  char* fragm){
        //-- получаем номер текущей записи
    int     dest_nom = getNom( dbhadr );
        //-- добавляем в таблицу новый фрагмент
    addFragm( dbhadr, fragm );
        //-- перемещаем его в бывшую текущую позицию
    dest_nom = moveFragm(  dbhadr, dest_nom );
    return  dest_nom;
};
//----------------------------------------------------------


//== скопировать базу фрагментов под другим именем
//--  возвращает количество скопированых записей
//--  или 0 при ошибке
//-- при  flag_pack != 0 удаленные записи не копируются

int    copyDBSN( char*  fil_name, char*  copy_filname, int flag_pack ) {
    int     res,
            new_count = 0;  //-- число скопированых записей
    int     rec_size = 0,
            max_recsize = 0,
            rec_nom,
            rec_count;
    int     dbhadr = 0,
            dbhnewadr = 0;

    //== расчитываем длину буфера фрагмента
    dbhadr = openDBSNA( fil_name );
    if( !dbhadr )   return 0;
    int*    dbh = ( int* )dbhadr;
    if( dbh[3] > FATAL_ERROR)    return 0;
    dbh[3] = 0; //-- стираем предупреждения
        //-- определяем исходное количество записей
    rec_count = countFragmA( dbhadr );
    if( rec_count == 0 )  {
        closeDBSN( dbhadr );
        return 0;
    }
        //-- проходим по всем записям
    for( rec_nom = 1; rec_nom <= rec_count; rec_nom++)  {
        setNom( dbhadr, rec_nom );
        res = getFragmSize(dbhadr, rec_size);
        if( rec_size < 0 )  rec_size = - rec_size;
        if( max_recsize < rec_size ) max_recsize = rec_size;
    }
        //-- формируем буфер фрагмента
    int     buf_size = max_recsize + 16;
    char    *fragm;
    fragm   = new char[buf_size];

    //== копируем исходную базу в результирующую
        //-- открываем результирующую базу
    res = createDBSN( copy_filname );
    if( !res ) {
        closeDBSN( dbhadr );
        return 0;
    }

    dbhnewadr = openDBSNA( copy_filname );
    if( !dbhnewadr )   return 0;

        //-- в цикле копируем все неудаленные записи
    for( rec_nom = 1; rec_nom <= rec_count; rec_nom++)  {
        res = setNom( dbhadr, rec_nom );
        res = getFragmSize( dbhadr, rec_size );
        if(( res <= 0 )&&( flag_pack ))  continue;

        res = getFragm(  dbhadr, fragm, buf_size );
        res = addFragm(dbhnewadr, fragm );
        new_count++;
    };
    //== закрываем базы и завершаем работу
    closeDBSN( dbhadr );
    closeDBSN( dbhnewadr );
    return  new_count;
};
//----------------------------------------------------------

//== Упаковать базу DBSN
//--  физически удаляет фрагменты с отрицательными длинами
//--  возвращает новое количество записей
//--  или 0 при ошибке
int  packDBSN( char* fil_name ) {
        //-- делаем временное имя новой базы
    char    temp_fil[200];
    strcpy( temp_fil, tmpnam ( "0000" ));
        //-- копируем в нее старую с флагом упаковки
    int     new_count = copyDBSN( fil_name, temp_fil, 1 );
    if( new_count ) {
        int     res = eraseDBSN(fil_name);
        if( !res )  return 0;
        res = renameDBSN(temp_fil, fil_name);
        if( !res ) return 0;
    }
    //-- возвращаем число скопированных записей
    return  new_count;
};
//----------------------------------------------------------

//==  записать в буфер описание ошибки с комментарием
//--  отладочная функция времени выполнения
//--  для накоплении в буфере описания всех происходящих ошибок
//--  с их координатами, расшифровкой кода и комментариями
//--  возвращает размер незанятой части буфера
//--  или 0 если ошибки не было или буфер ошибок переполнен
//    первым параметром принимает либо обработчик таблицы
//    либо отрицательный код ошибки
//      err_buf - буфер в который добавляется запись об ошибке
//      buf_len - длина этого буфера
//      comment - возможный комментарий
int  getErrMes( int dbhadr, char* err_buf, int buf_len, char* comment){
    if( !dbhadr )   return 0;
    int     err_cod = 0;
    if( dbhadr > 0 ) { //-- передан обработчик таблицы
        int*    dbh = ( int* )dbhadr;
        int     err_cod = dbh[3];
        if( err_cod ==0 ) return 0;  //-- обрабатываем только коды ошибок
    } else  //-- передан сам отрицательный код
        err_cod = -dbhadr;

        //-- записываем расшифровку битов кода ошибки
    char*   pc = err_buf;
    int     ost_len = buf_len;
    char*   pc_loc = 0;
    int     loc_cod = err_cod;
    int     tmp_len = 0;
    int     j = 0;
    int     errors[] ={BAD_PTR, BAD_NUMBER, BAD_TABLE_HANDLE, BAD_TABLE_NAME, BUFFER_SMALL,
                       DBN_NOT_EXIST, DBS_NOT_EXIST  };
    char    *err_mess[] = {"bad pointer", "bad number", "bad table handle; ", "bad table name; ", 
	"buffer is small; ", "dbn file not exist; ", "dbs file not exist; " };
    *pc = 0;    //-- разрешаем корректную работу strcat

    if( loc_cod >= FATAL_ERROR ) {  //-- есть ошибки
        pc_loc = "errors: ";  tmp_len = strlen(pc_loc);
        if( tmp_len  >= ost_len ) return 0;
        strcat( err_buf, pc_loc );  ost_len -= tmp_len;

            //-- перебираем все возможные ошибки
        for( j = 0; j< sizeof(errors)/sizeof(int); j++ ) {
            if( loc_cod & errors[ j ] ){
                pc_loc = err_mess[ j ];  tmp_len = strlen(pc_loc);
                if( tmp_len + 20 >= ost_len ) return 0;
                strcat( err_buf, pc_loc );  ost_len -= tmp_len;
            }
        }

    }

        //-- проверяем на предупреждения
    int     warns[] = { REC_EMPTY, DBN_IS_EMPTY, OUT_OF_RANGE, REC_DELETED, BAD_SUBSTNUMBER };
    char*   warn_mes[] = { "record is empty; ", "table is empty; ",
                            "record number out of range; ","record deleted; ", "bad number of substring" };
    loc_cod = loc_cod &( FATAL_ERROR - 1 );
    if( loc_cod ) {  //-- есть предупреждения
        pc_loc = "wornings: ";  tmp_len = strlen(pc_loc);
        if( tmp_len + 20 >= ost_len ) return 0;
        strcat( err_buf, pc_loc );  ost_len -= tmp_len;
            //-- перебираем все возможные предупреждения
        for( j = 0; j< sizeof(warns)/sizeof(int); j++ ) {
            if( err_cod & warns[ j ] ){
                pc_loc = warn_mes[ j ];  tmp_len = strlen(pc_loc);
                if( tmp_len + 20 >= ost_len ) return 0;
                strcat( err_buf, pc_loc );  ost_len -= tmp_len;
            }
        }
    }

        //-- дозаписываем комментарий
    tmp_len = strlen( comment );
    if( tmp_len + 20 >= ost_len ) return 0;
    strcat( err_buf, comment );  ost_len -= tmp_len;

        //-- возвращаем размер остатка буфера
    return  ost_len;
};
//----------------------------------------------------------
//== 09 sen 2013 ==

//==  получить  признак ошибки DBSN  =======================
int  getErrCod( int dbhadr ) {
    if( !dbhadr )  return BAD_TABLE_HANDLE;
    int*    dbh = (int*) dbhadr;
    return  dbh[3];
}
//----------------------------------------------------------
//==  вспомогательные функции ====

//== Считать Смещение текущего фрагмента базы DBSN  ========
//--  возвращает  номер записи
//--  или -код ошибки при фатальной ошибке

//-- внутренняя функция (если нет фатальных ошибок)
int  getFragmOffsetIn( int*  dbh,  int &rec_offset ){
    int     rec_nom =   dbh[2];
    int     fhn =   dbh[0];
    //-- проверяем существование нужной записи
    fseek(  (FILE*)fhn,0,2);
    int     rec_count = ftell(  (FILE*) fhn ) /8;
    if( !rec_count )  {
        dbh[3] = DBN_IS_EMPTY;
        return  0;
    }
    //-- считываем координаты строчного фрагмента
    fseek(  (FILE*)fhn, (  rec_nom-1)*8, 0);
    rec_offset = freadInt(  fhn );
    return   rec_nom;
}
//----------------------------------------------------------

//-- экпортируемый вариант ---
int  getFragmOffset( int  dbhadr,  int &rec_offset ) {
    if( getErrCod(dbhadr) >= FATAL_ERROR ) return  - getErrCod(dbhadr);
    return getFragmOffsetIn( (int*)dbhadr,  rec_offset );
};
//----------------------------------------------------------

//----------------------------------------------------------

//== Считать Длину текущего фрагмента базы DBSN ===========
//--  возвращает номер записи
//--  или -код ошибки при фатальной ошибке
//-- внутренняя функция (если нет фатальных ошибок)
int  getFragmSizeIn( int* dbh, int &rec_size ){
    int     rec_nom =   dbh[2];
    int     fhn =   dbh[0];
    //-- проверяем существование нужной записи
    fseek(  (FILE*)fhn,0,2);
    int     rec_count = ftell( (FILE*) fhn ) /8;
    if( !  rec_count)
    {	  dbh[3] = DBN_IS_EMPTY;
        return  0;
    }
    //-- считываем координаты строчного фрагмента
    fseek(  (FILE*)fhn, (  rec_nom-1)*8 + 4, 0);
    rec_size = freadInt(   fhn );
    return   rec_nom;
}

//-- экпортируемый вариант ---
int  getFragmSize( int dbhadr, int &rec_size ) {
    if( getErrCod(dbhadr) >= FATAL_ERROR ) return  - getErrCod(dbhadr);
    return  getFragmSizeIn( (int*) dbhadr, rec_size );
};
//----------------------------------------------------------


//== проверить активна ли запись ========================
//--  возвращает номер текущей записи если она активна
//--  или -код ошибки при фатальной ошибке

//-- внутренняя функция (если нет фатальных ошибок)
int isActiveIn( int* dbh ) {
    dbh[3] = 0; //-- стираем предупреждения
    int     rec_nom =   dbh[2];
    int     fhn =   dbh[0];
    //-- проверяем существование нужной записи
    fseek(  (FILE*)fhn,0,2);
    int     rec_count = ftell( (FILE*)fhn ) /8;
    if( ! rec_count)
    {	dbh[3] = DBN_IS_EMPTY;
        return  0;
    }
    //-- считываем координаты строчного фрагмента
    fseek( (FILE*)fhn, (  rec_nom-1)*8+4, 0);
    int     rec_size = freadInt(   fhn );
    if(  rec_size < 0)
        return 0;
    return   rec_nom;
}

//-- экпортируемый вариант ---
int isActive( int dbhadr ) {
    if( getErrCod(dbhadr) >= FATAL_ERROR ) return  - getErrCod(dbhadr);
    return  isActiveIn( (int*) dbhadr );
}
//----------------------------------------------------------

//== очистить признак ошибки ===============================
int  clearError( int dbhadr ) {
    if( getErrCod(dbhadr) >= FATAL_ERROR ) return  - getErrCod(dbhadr);
    int*    dbh = (int*) dbhadr;
    dbh[3] = 0;
    return dbh[2];
}
//----------------------------------------------------------

//==  Сосчитать число записей в таблице DBSN
//--  возвращает общее количество записей
//-- (совпадает с номером последней записи)
//--  или отрицвтельный код фатальной ошибки

//-- внутренняя функция (если нет фатальных ошибок)
int countFragmAIn( int* dbh ) {
    dbh[3] = 0; //-- стираем предупреждения
    //-- вытаскиваем обработчики файлов
    int     fhn = dbh[0];
    fseek((FILE*)fhn,0,2);
    int     rec_count = ftell( (FILE*)fhn ) /8;
    if( !rec_count )
        dbh[3] = DBN_IS_EMPTY;
    return  rec_count;
}

//-- экпортируемый вариант ---
int countFragmA( int dbhadr ) {
    if( getErrCod(dbhadr) >= FATAL_ERROR ) return  - getErrCod(dbhadr);
    return  countFragmAIn( (int*) dbhadr );

};
//----------------------------------------------------------

//==  позиционирование  ===
//==  получить номер текущего фрагмента  ===================
//--  возвращает номер записи
//--  или отрицвтельный код фатальной ошибки

//-- внутренняя функция (если нет фатальных ошибок)
int  getNomIn(int* dbh ){
    return  dbh[2];  //-- must be correct
}

//-- экпортируемый вариант ---
int  getNom(int dbhadr ) {
    if( getErrCod(dbhadr) >= FATAL_ERROR ) return  - getErrCod(dbhadr);
    return  getNomIn((int*) dbhadr );
}
//----------------------------------------------------------

//==  установить номер текущего фрагмента  =================
//--  возвращает номер записи
//--  или отрицвтельный код фатальной ошибки
//--  или 0 при попытки выйти за границы

//-- внутренняя функция (если нет фатальных ошибок)
int  setNomIn(int* dbh, int new_nom ) {
    dbh[3] = 0;  //-- очищаем предупреждения
    int     fr_count = countFragmAIn( dbh );
    if( fr_count == 0 ){
        dbh[3] |= OUT_OF_RANGE;  dbh[2] = 0; return dbh[2];
    };
    dbh[2] = new_nom;  //-- обычный случай
    if( new_nom < 1)  {
    	dbh[2] = 1;
        dbh[3] |= OUT_OF_RANGE;
        return  0;
    }
    if( new_nom > fr_count )  {
        dbh[2] = fr_count;
        dbh[3] |= OUT_OF_RANGE;
        return  0;
    }
    return  dbh[2];
}

//-- экпортируемый вариант ---
int  setNom(int dbhadr, int new_nom ) {
    if( getErrCod(dbhadr) >= FATAL_ERROR ) return  - getErrCod(dbhadr);
    return  setNomIn( (int *) dbhadr, new_nom );
}
//----------------------------------------------------------

//==  увеличить номер текущего фрагмента  =================
//--  возвращает номер новой записи
//--  или отрицвтельный код фатальной ошибки
//--  или 0 при попытки выйти за границы( указатель устанавл на конец)

//-- внутренняя функция (если нет фатальных ошибок)
int  incNomIn( int* dbh, int  count  )  {
    int new_nom = dbh[2] + count;
    return setNomIn( dbh, new_nom );
}

//-- экпортируемый вариант ---
int  incNom(int dbhadr, int  count  )  {
    if( getErrCod(dbhadr) >= FATAL_ERROR ) return  - getErrCod(dbhadr);
    return   incNomIn( (int*) dbhadr, count  );
}
//----------------------------------------------------------


//==  уменьшить номер текущего фрагмента  =================
//--  возвращает номер новой записи
//--  или отрицвтельный код фатальной ошибки
//--  или 0 при попытки выйти за границы( указатель устанавл на конец)

//-- внутренняя функция (если нет фатальных ошибок)
int decNomIn(int* dbh, int  count  )  {
    int new_nom = dbh[2] - count;
    return setNomIn( dbh, new_nom );
}

//-- экпортируемый вариант ---
int decNom(int dbhadr, int  count  )  {
    if( getErrCod(dbhadr) >= FATAL_ERROR ) return  - getErrCod(dbhadr);
    return   decNomIn((int*) dbhadr, count );
}
//----------------------------------------------------------

//==  установить первый номер фрагмента  ===============
//--  возвращает номер новой записи
//--  или отрицвтельный код фатальной ошибки
//--  или 0 при попытки выйти за границы( указатель устанавл на конец)

//-- внутренняя функция (если нет фатальных ошибок)
int  setFirstIn(int* dbh )  {
	return  setNomIn( dbh, 1);
}

//-- экпортируемый вариант ---
int  setFirst(int dbhadr )  {
	return  setNom(dbhadr, 1);
}
//----------------------------------------------------------

//==  установить последний номер фрагмента  ============
//--  возвращает номер новой записи
//--  или отрицвтельный код фатальной ошибки
//--  или 0 при попытки выйти за границы( указатель устанавл на конец)

//-- внутренняя функция (если нет фатальных ошибок)
int  setLastIn(int* dbh ){
    int new_nom = countFragmAIn( dbh );
    return  setNomIn( dbh, new_nom);
}

//-- экпортируемый вариант ---
int  setLast(int dbhadr ) {
    if( getErrCod(dbhadr) >= FATAL_ERROR ) return  - getErrCod(dbhadr);
    return  setLastIn( (int*) dbhadr );
}
//----------------------------------------------------------

//==  чтение  ==============================================
//== Считать текущий фрагмент базы DBSN  ===================
//--  возвращает номер записи или
//--  или отрицвтельный код ошибки
//--  или 0 для пустой записи

//-- внутренняя функция (если нет фатальных ошибок)
int  getFragmIn( int* dbh, char* fragm, int bufsize ) {
    int     rec_nom = dbh[2];
    if ( !rec_nom ){ //-- проверяем существование нужной записи
        dbh[3] |= DBN_IS_EMPTY;  return 0;
    }
        //-- считываем координаты строчного фрагмента
    int     fhn = dbh[0];
    int     nom_off = (rec_nom-1)*8;
    fseek((FILE*)fhn, nom_off, 0);
    int     rec_offset = freadInt( fhn );
    int     rec_size = freadInt( fhn );
    if(rec_size == 0 )  { //--  запись пустая
    	*fragm = 0;  dbh[3] |= REC_EMPTY;   return  0;
    }
    if(rec_size < 0 )  {//--  запись удалена
    	rec_size = -rec_size;  dbh[3] |= REC_DELETED;
    }
    if( rec_size >= bufsize) { //-- не влезает в буфер
        dbh[3] |= BUFFER_SMALL;  return  -dbh[3];
    }
    //-- считываем сам фрагмент
    int     fhs = dbh[1];
    fseek((FILE*)fhs, rec_offset, 0);
    fread((void*) fragm, rec_size,1,(FILE*) fhs);
    fragm[rec_size]=0;
    if( dbh[3] ) return -dbh[3];
    return rec_nom;
}

//-- экпортируемый вариант ---
int  getFragm(  int dbhadr, char* fragm, int bufsize ){
    if( getErrCod(dbhadr) >= FATAL_ERROR ) return  - getErrCod(dbhadr);
    return  getFragmIn( (int*) dbhadr, fragm, bufsize );
};
//----------------------------------------------------------

//== Удалить фрагмент базы DBSN с заданным номером
//-- просто делает его длину отрицательной
//--  возвращает номер записи
//--  или отрицвтельный код фатальной ошибки
//--  или 0 для пустой записи

//-- внутренняя функция (если нет фатальных ошибок)
int  delFragmIn(  int* dbh ) {
	dbh[3] = 0; //-- стираем предупреждения
		//-- проверяем существование нужной записи
	if(! countFragmAIn( dbh ))  return  0;
    int     rec_nom = dbh[2];
		//-- делаем отрицательной длину строчного фрагмента
    int     fhn = dbh[0];
	fseek((FILE*)fhn, (rec_nom-1)*8+4, 0);
	int     rec_size = freadInt( fhn );
	if(rec_size > 0) {
	   	rec_size = -rec_size;
		fseek((FILE*)fhn, (rec_nom-1)*8+4, 0);
		fwriteInt( fhn, rec_size );
        dbh[3] |= REC_DELETED;
    }
	return rec_nom;
}

//-- экпортируемый вариант ---
int  delFragm(  int dbhadr ) {
    if( getErrCod(dbhadr) >= FATAL_ERROR ) return  - getErrCod(dbhadr);
    return  delFragmIn(  (int*) dbhadr );
};
//----------------------------------------------------------


//== Снять признак удаления с фрагмента базы DBSN
//-- просто делает его длину неотрицательной
//--  возвращает номер записи
//--  или отрицвтельный код фатальной ошибки
//--  или 0 для пустой записи

//-- внутренняя функция (если нет фатальных ошибок)
int  undelFragmIn(  int* dbh )  {
	dbh[3] = 0; //-- стираем предупреждения
		//-- проверяем существование нужной записи
	if(! countFragmAIn( dbh ))  return  0;
		//-- делаем неотрицательной длину строчного фрагмента
    int     rec_nom = dbh[2];
    int     fhn = dbh[0];
	fseek((FILE*)fhn, (rec_nom-1)*8+4, 0);
	int     rec_size = freadInt( fhn );
	if(rec_size < 0)  {
		rec_size = -rec_size;
		fseek((FILE*)fhn, (rec_nom-1)*8+4, 0);
		fwriteInt( fhn, rec_size );
        dbh[3] &= ~REC_DELETED;
    }
	return rec_nom;
};


//-- экпортируемый вариант ---
int  undelFragm(  int dbhadr )  {
    if( getErrCod(dbhadr) >= FATAL_ERROR ) return  - getErrCod(dbhadr);
    return  undelFragmIn(  (int*) dbhadr );
};
//----------------------------------------------------------

//== работа с блоками фрагментов  ==========================
//-- номерация блоков от 1 до  countNblocks включительно

//==  Сосчитать число блоков записей в Базе DBSN
//--  возвращает общее количество блоков с учетом последнего незаполн
//--  или отрицвтельный код фатальной ошибки
//--  или 0 для пустой таблицы
//--  block_size - размер блока
//--  flag_sel - способ учета фрагментов
//--  ALL - все записи, ACTIVE - только активные
//--  не сдвигает внутренний номер записи
int countNblocks( int dbhadr, int block_size, int flag_sel = ACTIVE_FRAGM ) {
        //-- отрабатываем фатальную ошибку
    if( getErrCod(dbhadr) >= FATAL_ERROR ) return  - getErrCod(dbhadr);

    int*  dbh = (int*) dbhadr;
    dbh[3] = 0; //-- стираем предупреждения
        //-- считаем число записей
    int     fhn = dbh[0];  //-- вытаскиваем обработчики файлов
    fseek((FILE*)fhn,0,2);
    int     rec_count = ftell( (FILE*)fhn ) /8;
    if( !rec_count )  {   dbh[3] = DBN_IS_EMPTY;  return 0; }

    if(( flag_sel == ACTIVE_FRAGM)||
       ( flag_sel == ACTIVE_BACK))  {  //-- учитваем только активные записи
        int     active_count = 0,  rec_size;
        for( int rec_nom = 1; rec_nom <= rec_count; rec_nom++) {
                //-- считываем координаты строчного фрагмента
            fseek( (FILE*)fhn, (  rec_nom-1)*8+4, 0);
            rec_size = freadInt(   fhn );
            if(  rec_size > 0) active_count++;
        }
        rec_count =  active_count;
        if( !rec_count )  return 0;
    }
//-- считаем число непустых блоков
    int     block_count = rec_count / block_size;
    if( rec_count >  block_count *  block_size)
        block_count++;
    return  block_count;
};
//----------------------------------------------------------


//== получить блок( массив номеров ) фрагментов по его номеру
//-- возвращает реальный размер блока <= block_size
//-- при неправильном номере блока возвращает 0
int getNblock( int dbhadr, int* pblock, int block_nom, int block_size, int flag_sel = ACTIVE_FRAGM ) {
        //-- отрабатываем фатальную ошибку и выход за границы
    if( getErrCod(dbhadr) >= FATAL_ERROR ) return  - getErrCod(dbhadr);
    if(  block_nom < 1 )  return 0;  //-- ощибка
    if( block_nom > countNblocks( dbhadr, block_size, flag_sel ) ) return 0;
        //-- считаем число записей
    int*  dbh = (int*) dbhadr;
    dbh[3] = 0; //-- стираем предупреждения
    int     fhn = dbh[0];  //-- вытаскиваем обработчики файлов
    fseek((FILE*)fhn,0,2);
    int     rec_count = ftell( (FILE*)fhn ) /8;
    int rec_nom,
        rec_size,
        active_nom = 0,
        count_nom = 0,
        beg_nom = 1;
        //-- рассматриваем разные варианты выборки
    switch( flag_sel ){
        case  ALL_FRAGM:  //-- учитываем все записи вперед
            rec_nom = ( block_nom - 1) * block_size + 1;
            while( block_size ) {
                *pblock++ = rec_nom++;
                block_size--;  count_nom++;
                if( rec_nom > rec_count)  break;
            }
            break;
        case  ACTIVE_FRAGM:  //-- вперед только активные
                //-- ищем начало блока
            beg_nom = ( block_nom - 1) * block_size + 1;
            for( rec_nom = 1; rec_nom <= rec_count; rec_nom++) {
                    //-- считываем координаты строчного фрагмента
                fseek( (FILE*)fhn, (  rec_nom-1)*8+4, 0);
                rec_size = freadInt(   fhn );
                if(  rec_size > 0) active_nom++;
                if(  active_nom == beg_nom )  break;
            }
                //-- считываем номера активных записей блока
            while( block_size )  {
                *pblock++ = rec_nom++;
                block_size--;  count_nom++;
                for( ; rec_nom <= rec_count; rec_nom++) {
                        //-- считываем координаты строчного фрагмента
                    fseek( (FILE*)fhn, (  rec_nom-1)*8+4, 0);
                    rec_size = freadInt(   fhn );
                    if(  rec_size > 0)  break;
                }
                if( rec_nom > rec_count)  break;
            }
            break;
        case  ALL_BACK:
            rec_nom = rec_count - ( block_nom - 1) * block_size;
            while( block_size ) {
                *pblock++ = rec_nom--;
                block_size--;  count_nom++;
                if( rec_nom < 1 )  break;
            }
            break;
        case  ACTIVE_BACK:
                //-- ищем начало блока
            beg_nom = ( block_nom - 1) * block_size + 1;
            for( rec_nom = rec_count; rec_nom >= 1; rec_nom--) {
                    //-- считываем координаты строчного фрагмента
                fseek( (FILE*)fhn, (  rec_nom-1)*8+4, 0);
                rec_size = freadInt(   fhn );
                if(  rec_size > 0) active_nom++;
                if(  active_nom == beg_nom )  break;
            }
                //-- считываем номера активных записей блока
            while( block_size )  {
                *pblock++ = rec_nom--;
                block_size--;  count_nom++;
                for( ; rec_nom >= 1; rec_nom--) {
                        //-- считываем координаты строчного фрагмента
                    fseek( (FILE*)fhn, (  rec_nom-1)*8+4, 0);
                    rec_size = freadInt(   fhn );
                    if(  rec_size > 0)  break;
                }
                if( rec_nom < 1)  break;
            }
            break;
    };
    return  count_nom;
};
//----------------------------------------------------------

//-- новая реализация записи фрагментов  через бинарные ----

//==  Добавить бинарную запись в базу фрагментов DBSN
//--  возвращает номер добавленой записи(равный их количеству)
//--  или -код фатальной ошибки

//-- внутренняя функция (если нет фатальных ошибок)
int  addBinFragmIn( int* dbh, char* fragm, int len_fragm) {
    dbh[3] = 0; //-- стираем предупреждения
        //-- вытаскиваем обработчики файлов
    int     fhn = dbh[0];
    int     fhs = dbh[1];
        //-- дозаписываем фрагмент
    fseek((FILE*)fhs,0,2);
    int     rec_offset = ftell( (FILE*)fhs );
    fwrite( (void*)fragm, len_fragm, 1, (FILE*)fhs );
        //-- дозаписываем числовую информацию
    fseek((FILE*)fhn,0,2);
    fwriteInt( fhn, rec_offset);
    fwriteInt( fhn, len_fragm);
    fseek((FILE*)fhn,0,2);
    int     rec_count = ftell( (FILE*)fhn ) /8;
    dbh[2] = rec_count;
    return dbh[2];
};

//-- экпортируемый вариант ---
int  addBinFragm( int dbhadr, char* fragm, int len_fragm ) {
    if( getErrCod(dbhadr) >= FATAL_ERROR ) return  - getErrCod(dbhadr);
    return  addBinFragmIn( (int*) dbhadr, fragm, len_fragm );
};
//----------------------------------------------------------

//==  Добавить текстовую запись в базу фрагментов DBSN
//--  возвращает номер добавленой записи(равный их количеству)
//--  или -код фатальной ошибки
int  addFragm( int dbhadr, char* fragm ) {
    int     len_fragm = getLen( fragm );
    return  addBinFragm( dbhadr, fragm, len_fragm );
};
//----------------------------------------------------------

//==  Записать Бинарный фрагмент в текущую запись базы DBSN
//--  возвращает номер записи
//--  или -код фатальной ошибки
//--  или 0 при записи вне границ

int  setBinFragm(  int dbhadr, char* fragm, int len_fragm ) {
    if( getErrCod(dbhadr) >= FATAL_ERROR ) return  - getErrCod(dbhadr);
    int* dbh = (int*) dbhadr;
    int     rec_nom = dbh[2];
        //-- проверяем существование нужной записи
    int     fr_count = countFragmAIn( dbh );
    if((! fr_count )||(rec_nom < 1)||(rec_nom > fr_count)) {
        dbh[3] |= OUT_OF_RANGE;
        return  0;
    }
        //-- дозаписываем собственно фрагмент
    int     fhn = dbh[0];
    int     fhs = dbh[1];
    fseek((FILE*)fhs,0,2);
    int     rec_offset = ftell( (FILE*)fhs );
    fwrite((void*)fragm, len_fragm, 1, (FILE*)fhs);
        //-- перезаписываем числовую информацию
    fseek((FILE*)fhn, (rec_nom-1)*8, 0);
    fwriteInt( fhn, rec_offset);
    fwriteInt( fhn, len_fragm);
    return rec_nom;
}
//----------------------------------------------------------


//==  Записать текст в текущую запись базы DBSN
//--  возвращает номер записи
//--  или -код фатальной ошибки
//--  или 0 при записи вне границ
int  setFragm(  int dbhadr, char* fragm ) {
	int     len_fragm = getLen( fragm );
    return  setBinFragm(  dbhadr, fragm, len_fragm );
};
//----------------------------------------------------------

//== active functions  ==

//==  перейти к первому активному фрагменту
//--    возвращает номер фрагмента
//--    или -код фатальной ошибки
//--    или 0 при невозможности ( нет активных записей )
int    gotoFirst( int dbhadr ){
    if( getErrCod(dbhadr) >= FATAL_ERROR ) return  - getErrCod(dbhadr);
    int*  dbh = (int*) dbhadr;
    dbh[3] = 0; //-- стираем предупреждения

    int     fhn = dbh[0];  //-- вытаскиваем обработчики файлов
        //-- считаем число записей
    fseek((FILE*)fhn,0,2);
    int     rec_count = ftell( (FILE*)fhn ) /8;
        //--  ищем первый активный элемент
    int rec_nom,
        rec_size;
    for( rec_nom = 1; rec_nom <= rec_count; rec_nom++)  {
            //-- считываем координаты строчного фрагмента
        fseek( (FILE*)fhn, (  rec_nom-1)*8+4, 0);
        rec_size = freadInt(   fhn );
        if(  rec_size > 0) {
            dbh[2] = rec_nom;
            return  rec_nom;
        };
    }
    return  0;  //-- не нашли
};
//----------------------------------------------------------

//==  перейти вперед через несколько активных фрагментов
//--    возвращает номер фрагмента
//--    или -код фатальной ошибки
//--    или 0 при невозможности ( нет столько активных записей )
int    goForward( int dbhadr, int step_count ){
    if( getErrCod(dbhadr) >= FATAL_ERROR ) return  - getErrCod(dbhadr);
    int*  dbh = (int*) dbhadr;
    dbh[3] = 0; //-- стираем предупреждения

    int     fhn = dbh[0];  //-- вытаскиваем обработчики файлов
        //-- считаем число записей
    fseek((FILE*)fhn,0,2);
    int     rec_count = ftell( (FILE*)fhn ) /8;
    int rec_nom,
        rec_size;

        //--  пытаемся сдвинуть указатель
    for( rec_nom = dbh[2]; rec_nom <= rec_count; rec_nom++)  {
            //-- считываем координаты строчного фрагмента
        fseek( (FILE*)fhn, (  rec_nom-1)*8+4, 0);
        rec_size = freadInt(   fhn );
        if(  rec_size > 0) {
            dbh[2] = rec_nom;
            if( step_count >0) {
                step_count--;  continue;
            }
            return  rec_nom;
        };
    }
    return  0;
};
//----------------------------------------------------------

//==  перейти назад через несколько активных фрагментов
//--    возвращает номер фрагмента
//--    или -код фатальной ошибки
//--    или 0 при невозможности ( нет столько активных записей )
int    goBack( int dbhadr, int step_count ){
    if( getErrCod(dbhadr) >= FATAL_ERROR ) return  - getErrCod(dbhadr);
    int*  dbh = (int*) dbhadr;
    dbh[3] = 0; //-- стираем предупреждения

    int     fhn = dbh[0];  //-- вытаскиваем обработчики файлов
    int rec_nom,
        rec_size;
        //--  пытаемся сдвинуть указатель
    for( rec_nom = dbh[2]; rec_nom >= 1; rec_nom--)  {
            //-- считываем координаты строчного фрагмента
        fseek( (FILE*)fhn, (  rec_nom-1)*8+4, 0);
        rec_size = freadInt(   fhn );
        if(  rec_size > 0) {
            dbh[2] = rec_nom;
            if( step_count > 0) {
                step_count--;  continue;
            }
            return  rec_nom;
        };
    }
    return  0;
};
//----------------------------------------------------------

//==  перейти к последнему активному фрагменту
//--    возвращает номер фрагмента
//--    или -код фатальной ошибки
//--    или 0 при невозможности ( нет активных записей )
int    gotoLast( int dbhadr ){
    if( getErrCod(dbhadr) >= FATAL_ERROR ) return  - getErrCod(dbhadr);
    int*  dbh = (int*) dbhadr;
    dbh[3] = 0; //-- стираем предупреждения

    int     fhn = dbh[0];  //-- вытаскиваем обработчики файлов
        //-- считаем число записей
    fseek((FILE*)fhn,0,2);
    int     rec_count = ftell( (FILE*)fhn ) /8;
    int     rec_nom,
            rec_size;

        //--  ищем последний активный элемент
    for( rec_nom = rec_count; rec_nom >= 1; rec_nom--)  {
            //-- считываем координаты строчного фрагмента
        fseek( (FILE*)fhn, (  rec_nom-1)*8+4, 0);
        rec_size = freadInt(   fhn );
        if(  rec_size > 0) {
            dbh[2] = rec_nom;
            return  rec_nom;
        };
    }
    return  0;
};
//----------------------------------------------------------


//== 28 sen 2013 ===========================================
//-- доп функции работы с подстроками

//== добавить подстроку к строке сзади
//-- результат записать в буфер pcres
//-- возвращает длину результата или отриц код ошибки
int  addSubst(char* pc, char* pcres, int res_size, char* pcsubst) {
        //-- выполняем проверки
    if( !pc || !pcres || !pcsubst ) return - BAD_PTR;
    int     st_len = getLen( pc),
            subst_len = getLen( pcsubst );
    if( res_size < st_len + subst_len + 2 ) return - BUFFER_SMALL;
       //-- дописываем строку
    char    *pc_src = pc, *pc_dst = pcres;
    while( *pc_src )
        *pc_dst++ = *pc_src++;
        //-- дописываем новую подстроку с проверкой на начало
    if( pc_dst != pcres )
        *pc_dst++ = '|';
    while( *pcsubst )
        *pc_dst++ = *pcsubst++;
    *pc_dst = 0;
        //-- возвращаем длину полученой строки
    return pc_dst - pcres - 1;
};
//----------------------------------------------------------

//-- вставить новую подстроку в нужное место строки
//-- результат поместить в буфер pcres
//-- возвращает длину результата или отриц код ошибки
int insSubst(char* pc, char* pcres, int res_size, int nom_sub, char* pcsubst) {
        //-- выполняем проверки
    if( !pc || !pcres || !pcsubst ) return - BAD_PTR;
    int     st_len = getLen( pc),
            subst_len = getLen( pcsubst );
    if( res_size < st_len + subst_len + 2 ) return - BUFFER_SMALL;
    int     subst_count = substCount(pc);
    if( nom_sub <0) nom_sub += subst_count; //-- отриц считаем с конца
    if( (nom_sub <0)||(nom_sub >= subst_count))
        return  - BAD_NUMBER;
        //-- копируем в буфер источник до нужной подстроки
    char    *pc_src = pc, *pc_dst = pcres;
    if(pc[st_len-1]=='\n')  pc[st_len-1]=0; //-- корректиируем последний конец строки
    *pcres = 0;
    while( nom_sub ){ //-- копируем до разделителя включительно
        if( *pc_src =='|')
            nom_sub--;
        *pc_dst++ = *pc_src++;
    }
        //-- копируем новую подстроку с разделителем
    while( *pcsubst ){
        *pc_dst++ = *pcsubst++;
    }
    *pc_dst++ = '|';
        //-- копируем остаток исходной строки
    while( *pc_src ){
        *pc_dst++ = *pc_src++;
    }
    *pc_dst = 0;
    return pc_dst - pcres - 1; //--возвращаем результат
};
//----------------------------------------------------------

//-- вычислить длину заданной подстроки
//-- возвращает длину подстроки или отриц код ошибки
int substLen(char* pc, int nom_sub ) {
        //-- выполняем проверки
    if( !pc  ) return - BAD_PTR;
    int     subst_count = substCount(pc);
    if( nom_sub <0) nom_sub += subst_count; //-- отриц считаем с конца
    if( (nom_sub <0)||(nom_sub >= subst_count))
        return  - BAD_NUMBER;
        //-- ищем начало подстроки
    char    *pc_src = pc;
    while( nom_sub ){ //-- пропускаем начало до разделителя включительно
        if( *pc_src =='|')
            nom_sub--;
        pc_src++;
    }
        //-- ищем конец подстроки
    char*  pc_beg = pc_src;
    while( (*pc_src != '|') &&( *pc_src != 0))
        pc_src++;
        //-- возвращаем ее длину
    return  pc_src - pc_beg;
}
//----------------------------------------------------------

//-- заменить подстроку на новое значение в нужном месте строки
//-- результат поместить в буфер pcres
//-- возвращает длину результата или отриц код ошибки
int setSubst(char* pc, char* pcres, int res_size, int nom_sub, char* pcsubst) {
        //-- выполняем проверки
    if( !pc || !pcres || !pcsubst ) return - BAD_PTR;
    int     subst_count = substCount(pc);
    if( nom_sub <0) nom_sub += subst_count; //-- отриц считаем с конца
    if( (nom_sub <0)||(nom_sub >= subst_count))
        return  - BAD_NUMBER;
    int     st_len = getLen( pc),
            oldsubst_len = substLen( pc, nom_sub ),
            newsubst_len = getLen( pcsubst );
    if( res_size < st_len + newsubst_len - oldsubst_len + 2 ) return - BUFFER_SMALL;
        //-- копируем в буфер источник до нужной подстроки
    char    *pc_src = pc, *pc_dst = pcres;
    if(pc[st_len-1]=='\n')  pc[st_len-1]=0; //-- корректиируем последний конец строки
    *pcres = 0;
    while( nom_sub ){ //-- копируем до разделителя включительно
        if( *pc_src =='|')
            nom_sub--;
        *pc_dst++ = *pc_src++;
    }
        //-- копируем новую подстроку с разделителем
    while( *pcsubst ){
        *pc_dst++ = *pcsubst++;
    }
    //*pc_dst++ = '|';
        //-- пропускаем старую подстроку
    while( (*pc_src != '|') &&( *pc_src != 0))
        pc_src++;
        //-- копируем остаток исходной строки
    while( *pc_src ){
        *pc_dst++ = *pc_src++;
    }
    *pc_dst = 0;
    return pc_dst - pcres - 1; //--возвращаем результат
};

//----------------------------------------------------------

//-- очистить подстроку (заменить на пустое значение )
//-- возвращает длину получившейся строки или отриц код ошибки
int clearSubst(char* pc, int nom_sub ) {
        //-- выполняем проверки
    if( !pc ) return - BAD_PTR;
    int     subst_count = substCount(pc);
    if( nom_sub <0) nom_sub += subst_count; //-- отриц считаем с конца
    if( (nom_sub <0)||(nom_sub >= subst_count))
        return  - BAD_NUMBER;
    int     st_len = getLen( pc);
    char    *pc_src = pc;
    if(pc[st_len-1]=='\n')  pc[st_len-1]=0; //-- корректиируем последний конец строки
        //-- находим начало нужной подстроки
    while( nom_sub ){
        if( *pc_src =='|')
            nom_sub--;
        pc_src++;
    }
        //-- пропускаем содержимое подстроки до разделителя
    char*    pc_dst = pc_src;
    while( (*pc_src != '|') &&( *pc_src != 0))
        pc_src++;
        //-- копируем остаток исходной строки
    while( *pc_src ){
        *pc_dst++ = *pc_src++;
    }
    *pc_dst = 0;
    return pc_dst - pc - 1; //--возвращаем результат
};
//----------------------------------------------------------

//-- полностью удалить подстроку
//-- возвращает длину получившейся строки или отриц код ошибки
int cutSubst(char* pc, int nom_sub ) {
        //-- выполняем проверки
    if( !pc ) return - BAD_PTR;
    int     subst_count = substCount(pc);
    if( nom_sub <0) nom_sub += subst_count; //-- отриц считаем с конца
    if( (nom_sub <0)||(nom_sub >= subst_count))
        return  - BAD_NUMBER;
    int     st_len = getLen( pc);
    char    *pc_src = pc;
    if(pc[st_len-1]=='\n')  pc[st_len-1]=0; //-- корректиируем последний конец строки
        //-- находим начало нужной подстроки
    while( nom_sub ){
        if( *pc_src =='|')
            nom_sub--;
        pc_src++;
    }
        //-- пропускаем содержимое подстроки до разделителя
    char*    pc_dst = pc_src;
    while( (*pc_src != '|') &&( *pc_src != 0))
        pc_src++;
    if( *pc_src == '|')
        pc_src++;  //-- стираем разделитель
        //-- копируем остаток исходной строки
    while( *pc_src ){
        *pc_dst++ = *pc_src++;
    }
    *pc_dst = 0;
    return pc_dst - pc - 1; //--возвращаем результат
};
//----------------------------------------------------------

//-- полностью удалить группу подстрок
//-- возвращает длину получившейся строки или отриц код ошибки
int cutSubstGroup(char* pc, int nom_sub, int cut_count = 1 ) {
        //-- выполняем проверки
    if( !pc ) return - BAD_PTR;
    int     subst_count = substCount(pc);
    if( nom_sub <0) nom_sub += subst_count; //-- отриц считаем с конца
    if( (nom_sub <0)||(nom_sub >= subst_count))
        return  - BAD_NUMBER;
    int     st_len = getLen( pc);
    char    *pc_src = pc;
    if(pc[st_len-1]=='\n')  pc[st_len-1]=0; //-- корректиируем последний конец строки
        //-- находим начало нужной подстроки
    while( nom_sub ){
        if( *pc_src =='|')
            nom_sub--;
        pc_src++;
    }
        //-- проверяем на допустимое число подстрок
    if( nom_sub + cut_count >= subst_count){
        if( nom_sub > 0)   pc_src--;
        *pc_src = 0;
        return pc_src - pc;
    }
        //-- пропускаем содержимое подстроки до нужного разделителя
    char*    pc_dst = pc_src;
    while( cut_count ){
        if( *pc_src == '|')   cut_count--;
        pc_src++;
    }
        //-- копируем остаток исходной строки
    while( *pc_src ){
        *pc_dst++ = *pc_src++;
    }
    *pc_dst = 0;
    return pc_dst - pc - 1; //--возвращаем результат
};
//----------------------------------------------------------
//-- удалить все подстроки перед заданой
//-- возвращает длину получившейся строки или отриц код ошибки
int cutLeft(char* pc, int nom_sub ) {
        //-- выполняем проверки
    if( !pc ) return - BAD_PTR;
    int     subst_count = substCount(pc);
    if( nom_sub <0) nom_sub += subst_count; //-- отриц считаем с конца
    if( (nom_sub <0)||(nom_sub >= subst_count))
        return  - BAD_NUMBER;
    int     st_len = getLen( pc);
    char    *pc_src = pc;
    if(pc[st_len-1]=='\n')  pc[st_len-1]=0; //-- корректиируем последний конец строки
        //-- находим начало нужной подстроки
    while( nom_sub ){
        if( *pc_src =='|')
            nom_sub--;
        pc_src++;
    }
        //-- пропускаем содержимое подстроки до разделителя
    char*    pc_dst = pc;
        //-- копируем остаток исходной строки
    while( *pc_src ){
        *pc_dst++ = *pc_src++;
    }
    *pc_dst = 0;
    return pc_dst - pc - 1; //--возвращаем результат
};

//----------------------------------------------------------

//-- удалить все подстроки начиная с заданой
//-- возвращает длину получившейся строки или отриц код ошибки
int cutRight(char* pc, int nom_sub ) {
        //-- выполняем проверки
    if( !pc ) return - BAD_PTR;
    int     subst_count = substCount(pc);
    if( nom_sub <0) nom_sub += subst_count; //-- отриц считаем с конца
    if( (nom_sub <0)||(nom_sub >= subst_count))
        return  - BAD_NUMBER;
    int     st_len = getLen( pc);
    char    *pc_src = pc;
    if(pc[st_len-1]=='\n')  pc[st_len-1]=0; //-- корректиируем последний конец строки
    if( nom_sub == 0 ){ //-- вырожденный случай - удаляем всю строку
        *pc = 0;
        return  0;
    }
        //-- находим начало нужной подстроки
    while( nom_sub ){
        if( *pc_src =='|')
            nom_sub--;
        pc_src++;
    }
        //-- формируем конец строки
    pc_src--;
    *pc_src = 0;
    return pc_src - pc - 1;  //--возвращаем результат
};
//----------------------------------------------------------

//-- объединить подстроку с предидущей
//-- заменяет разделитель на пробел
//-- возвращает длину получившейся строки или отриц код ошибки
int uniteSubst(char* pc, int nom_sub ) {
        //-- выполняем проверки
    if( !pc ) return - BAD_PTR;
    int     subst_count = substCount(pc);
    if( nom_sub <0) nom_sub += subst_count; //-- отриц считаем с конца
    if( (nom_sub <0)||(nom_sub >= subst_count))
        return  - BAD_NUMBER;
    int     st_len = getLen( pc);
    char    *pc_src = pc;
    if(pc[st_len-1]=='\n'){
         pc[st_len-1]=0; //-- корректиируем последний конец строки
         st_len--;
    }
    if( !nom_sub ){  //-- для нулевой подстроки нет предидущей
        return  st_len;
    }
        //-- находим начало нужной подстроки
    while( nom_sub ){
        if( *pc_src =='|')
            nom_sub--;
        pc_src++;
    }
        //-- заменяем разделитель на пробел
    pc_src--;  *pc_src = ' ';
    return  st_len;  //--возвращаем результат
};
//----------------------------------------------------------

//-- 27 jan 2014 ---
//-- обаработка бинарных подстрок числами(словами)

//== установить новое число в заданное место нужной бинарной подстроки
//-- возвращает длину фрагмента или отриц ошибку
//== внутренний вариант без проверки на ошибку
int 
setBinnumIn(char* pcFragm, int fragmLen, long nom_sub, long nom_num, long src ){
    int     offset = binsubstOffsetIn( pcFragm, fragmLen, nom_sub) + nom_num*4;
    int*    pl = (int*)(pcFragm + offset);
    *pl = src;
    return  fragmLen;
}
//== внешний вариант
int 
setBinnum(char* pcFragm, int fragmLen, long nom_sub, long nom_num, long src ){
    if (( !pcFragm )||( !fragmLen))  return - BAD_PTR;
    if( nom_sub > binsubstCountIn( pcFragm, fragmLen) )  return -BAD_NUMBER;
    if( nom_num >= binsubstSizeIn( pcFragm, fragmLen, nom_sub ))  return -BAD_NUMBER;
    if( nom_num <= 0 )  return -BAD_NUMBER;
    return  setBinnumIn( pcFragm, fragmLen, nom_sub, nom_num, src );
}
//----------------------------------------------------------

//== получить число с заданным номером из нужной бинарной подстроки
//-- возвращает длину фрагмента или отриц ошибку
//-- а по ссылке dst - число если не было ошибки
int 
getBinnumIn(char* pcFragm, int fragmLen, long nom_sub, long nom_num, long &dst ){
    int     offset = binsubstOffsetIn( pcFragm, fragmLen, nom_sub) + nom_num*4;
    dst = *(int*)(pcFragm + offset);
    return  fragmLen;
}
//== внешний вариант
int 
getBinnum(char* pcFragm, int fragmLen, long nom_sub, long nom_num, long &dst ){
    if (( !pcFragm )||( !fragmLen))  return - BAD_PTR;
    if( nom_sub > binsubstCountIn( pcFragm, fragmLen) )  return -BAD_NUMBER;
    if( nom_num >= binsubstSizeIn( pcFragm, fragmLen, nom_sub ))  return -BAD_NUMBER;
    if( nom_num <= 0 )  return -BAD_NUMBER;
    return  getBinnumIn( pcFragm, fragmLen, nom_sub, nom_num, dst );
}
//----------------------------------------------------------


//== удалить число с заданным номером из нужной бинарной подстроки
//-- возвращает длину фрагмента или отриц ошибку
//-- внутренний вариант
int 
cutBinnumIn(char* pcFragm, int fragmLen, long nom_sub, long nom_num ){
    int     offset = binsubstOffsetIn( pcFragm, fragmLen, nom_sub) + nom_num*4;
        //--сдвигаем влево на 4 байта правую часть фрагмента
    for( int  j = offset; j < fragmLen - 4; j++) {
        pcFragm[j] = pcFragm[j+4];
    }
        //-- уменьшаем длины фрагмента и подстроки
    offset = binsubstOffsetIn( pcFragm, fragmLen, nom_sub);
    int*    pl = (int*) (pcFragm + offset );
    *pl -= 4;
    fragmLen -= 4;
    return  fragmLen;
}
//== внешний вариант ==
int 
cutBinnum(char* pcFragm, int fragmLen, long nom_sub, long nom_num ){
    if (( !pcFragm )||( !fragmLen))  return - BAD_PTR;
    if( nom_sub > binsubstCountIn( pcFragm, fragmLen) )  return -BAD_NUMBER;
    if( nom_num >= binsubstSizeIn( pcFragm, fragmLen, nom_sub ))  return -BAD_NUMBER;
    if( nom_num <= 0 )  return -BAD_NUMBER;
    return  cutBinnumIn( pcFragm, fragmLen, nom_sub, nom_num );
}
//----------------------------------------------------------

//== добавить новое число в конец нужной бинарной подстроки
//-- возвращает длину фрагмента или отриц ошибку
//== внутренний вариант без проверки на ошибку
int 
addBinnumIn(char* pcFragm, int fragmLen, char* pcBuf, int lenBuf, long nom_sub, long src ){
        //-- копируем в буфер нужную часть строки
    int     offset = binsubstOffsetIn( pcFragm, fragmLen, nom_sub );
    int     loc_nom = 0;
    for( loc_nom = 0; loc_nom < offset; loc_nom++ ){
        *pcBuf++ = *pcFragm++;
    }
        //-- добавляем новое число
    int*    pl = (int*)(pcFragm + offset);
    *pl = src;
        //-- дописываем хвост фрагмента
    pcBuf += 4;
    for( loc_nom = offset; loc_nom < fragmLen; loc_nom++ ){
        *pcBuf++ = *pcFragm++;
    }
        //-- уточняем длину построки и фрагмента
    offset = binsubstOffsetIn( pcFragm, fragmLen, nom_sub);
    pl = (int*) (pcFragm + offset );
    *pl += 4;
    fragmLen += 4;
    return  fragmLen;
}
//== внешний вариант
int 
addBinnum(char* pcFragm, int fragmLen, char* pcBuf, int lenBuf, long nom_sub, long src ){
    if (( !pcFragm )||( !fragmLen))  return - BAD_PTR;
    if( lenBuf < fragmLen + 4 ) return  -BUFFER_SMALL;
    if( nom_sub > binsubstCountIn( pcFragm, fragmLen) )  return -BAD_NUMBER;
    return  addBinnumIn( pcFragm, fragmLen, pcBuf, lenBuf, nom_sub, src );
}
//----------------------------------------------------------

//== вставить новое число в заданное место нужной бинарной подстроки
//-- возвращает длину фрагмента или отриц ошибку
//== внутренний вариант без проверки на ошибку
int 
insBinnumIn(char* pcFragm, int fragmLen, char* pcBuf, int lenBuf, long nom_sub, long nom_num, long src ){
    int     offset = binsubstOffsetIn( pcFragm, fragmLen, nom_sub) + nom_num*4;
        //-- копируем в буфер нужную часть строки
    int     loc_nom = 0;
    for( loc_nom = 0; loc_nom < offset; loc_nom++ ){
        *pcBuf++ = *pcFragm++;
    }
        //-- добавляем новое число
    int*    pl = (int*)(pcFragm + offset);
    *pl = src;
        //-- дописываем хвост фрагмента
    pcBuf += 4;
    for( loc_nom = offset; loc_nom < fragmLen; loc_nom++ ){
        *pcBuf++ = *pcFragm++;
    }
        //-- уточняем длину построки и фрагмента
    offset = binsubstOffsetIn( pcFragm, fragmLen, nom_sub);
    pl = (int*) (pcFragm + offset );
    *pl += 4;
    fragmLen += 4;
    return  fragmLen;
}
//== внешний вариант
int 
insBinnum(char* pcFragm, int fragmLen, char* pcBuf, int lenBuf, long nom_sub, long nom_num, long src ){
    if (( !pcFragm )||( !fragmLen))  return - BAD_PTR;
    if( lenBuf < fragmLen + 4 ) return  -BUFFER_SMALL;
    if( nom_sub > binsubstCountIn( pcFragm, fragmLen) )  return -BAD_NUMBER;
    if( nom_num >= binsubstSizeIn( pcFragm, fragmLen, nom_sub ))  return -BAD_NUMBER;
    if( nom_num <= 0 )  return -BAD_NUMBER;
    return  insBinnumIn( pcFragm, fragmLen, pcBuf, lenBuf, nom_sub, nom_num, src );
}
//----------------------------------------------------------


//== поменять местами 2 числа в нужной бинарной подстроке
//-- возвращает длину фрагмента или отриц ошибку
//== внутренний вариант без проверки на ошибку
int 
xchgBinnumIn(char* pcFragm, int fragmLen, long nom_sub, long nom_num1, long nom_num2 ){
    int     offset = binsubstOffsetIn( pcFragm, fragmLen, nom_sub);
    int*    pl = (int*)(pcFragm + offset);
    int*    pl1 = pl+nom_num1;
    int*    pl2 = pl+nom_num2;
    int     temp = *pl1;  *pl1 = *pl2;  *pl2 = temp;
    return  fragmLen;
}
//== внешний вариант
int 
xchgBinnum(char* pcFragm, int fragmLen, long nom_sub, long nom_num1, long nom_num2 ){
    if (( !pcFragm )||( !fragmLen))  return - BAD_PTR;
    if( nom_sub > binsubstCountIn( pcFragm, fragmLen) )  return -BAD_NUMBER;
    if( nom_num1 >= binsubstSizeIn( pcFragm, fragmLen, nom_sub ))  return -BAD_NUMBER;
    if( nom_num1 <= 0 )  return -BAD_NUMBER;
    if( nom_num2 >= binsubstSizeIn( pcFragm, fragmLen, nom_sub ))  return -BAD_NUMBER;
    if( nom_num2 <= 0 )  return -BAD_NUMBER;
    return  xchgBinnumIn( pcFragm, fragmLen, nom_sub, nom_num1, nom_num2 );
}
//----------------------------------------------------------
//== 31 jan 2014 ====
//== формирование таблицы  по текстовому файлу и слову-разделителю
//-- например, для разбиения книги на главы по слову "Глава"
//-- или для разбиения плана по строкам, тлт текста по параграфам
//-- вовращает количество фрагментов или отрицательный код ошибки
//-- флажок add_key показывает, надо ли включать само ключевое слово
//-- в состав фрагмента, принимает значения:
//       0 - исключается из обоих соседних фрагментов
//       1 - добавляется в состав предидущего фрагмента
//      -1 - добавляется в начало следующего фрагмента
int 
formDBSN( char* txtFile, char* keyWord, int add_key ){
    int     buf_size = 4096;  //--размер половинки буфера
    char    buf[9182];        //-- сам буфер для выполнения операций чтения
    char    fil_dbs[128];    //--имя DBS файла
    char    fil_dbn[128];    //--имя DBN файла
        //-- формируем имя DBS и DBN файла
    char*    pc_src = txtFile;
    char*    pc_dbs = fil_dbs;
    char*    pc_dbn = fil_dbn;
    while( (*pc_src)&&(*pc_src !='.')) {
        *pc_dbs++ = *pc_src;  *pc_dbn++ = *pc_src; pc_src++;
    }
    for( pc_src =".db"; *pc_src; pc_src++) {
        *pc_dbs++ = *pc_src; *pc_dbn++ = *pc_src;
    }
    *pc_dbs++ ='s'; *pc_dbs = 0; *pc_dbn++ ='n'; *pc_dbn = 0;

    FILE*   fsrc = fopen( txtFile, "rb");
    if(!fsrc) {               //-- не смогли открыть исходный файл
        return -DBS_NOT_EXIST;
    }
        //-- Считываем первую ( двойную ) порцию данных
    int     data_len = fread(buf, 1, 2*buf_size, fsrc );  //-- данных в буфере
    if( !data_len )  {
        fclose( fsrc );  return -DBS_NOT_EXIST;
    }
        //-- формируем DBS, DBN файлы
    FILE*     filhs =  fopen(fil_dbs, "wb");
    if(!filhs) {               //-- не смогли создать файл dbs
        return -DBS_NOT_EXIST;
    }
    int     fhn = (int)  fopen(fil_dbn, "wb");
    if(!fhn){               //-- не смогли создать файл dbn
        return -DBN_NOT_EXIST;
    }
        //-- начинаем обработку - переменные
    int     fragm_count = 0;
    int     keyword_len = strlen( keyWord );
    int     rec_offset = 0;
    int     fragm_len = 0;
    int     buf_offset = 0;  //-- позиция последнего ключевого слова в буфере
    int     loc_nom;
        //-- цикл по длинному файлу
    while( data_len == 2*buf_size){
            //-- записываем текст в dbs файл
        fwrite(buf, 1, buf_size, filhs );
            //-- просматриваем первую половину буфера
        for( loc_nom = 0; loc_nom < buf_size; loc_nom++ ){
            if( ! strncmp( buf+loc_nom, keyWord, keyword_len ) ) {
                    //-- нашли ключевое слово - записываем в зависимости от флага
                fwriteInt( fhn, rec_offset );
                switch( add_key ){
                    case   1:  //-- добавить ключевое слово к концу предидущ
                        fragm_len += loc_nom - buf_offset + keyword_len;
                        rec_offset += fragm_len ;
                        buf_offset = loc_nom + keyword_len;
                        break;
                    case  -1:  //-- добавить ключевое слово в начало следующего
                        fragm_len += loc_nom - buf_offset;
                        rec_offset += fragm_len;
                        buf_offset = loc_nom;
                        break;
                    default:   //-- не добавлять ключ слово к фрагментм
                        fragm_len += loc_nom - buf_offset;
                        rec_offset += fragm_len + keyword_len;
                        buf_offset = loc_nom + keyword_len;
                }
                fwriteInt( fhn, fragm_len );
                fragm_len = 0;
                fragm_count++;
            }
        }
            //-- сдвигаем вторую половину буферв к началу и уточняем размер фр
        for( loc_nom = 0; loc_nom < buf_size; loc_nom++)
            buf[ loc_nom ] = buf[ loc_nom + buf_size ];
        fragm_len += buf_size - buf_offset;
        buf_offset = 0;
            //-- дочитываем файл во вторую часть буфера
        data_len = buf_size + fread(buf + buf_size, 1, buf_size, fsrc );
    }
       //-- записываем остаток текста в dbs файл
    fwrite(buf, 1, data_len, filhs );
       //-- обработка последней части файла
    for( loc_nom = 0; loc_nom < data_len - keyword_len; loc_nom++ ){
        if( ! strncmp( buf+loc_nom, keyWord, keyword_len ) ) {
                //-- нашли ключевое слово - записываем
            fragm_len += loc_nom - buf_offset;
            fwriteInt( fhn, rec_offset );
            fwriteInt( fhn, fragm_len );
            rec_offset += fragm_len;
            fragm_len = 0;
            buf_offset = loc_nom;
            fragm_count++;
        }
    }
        //-- обработка конца файла ( за последним ключевым словом )
    fragm_len += data_len - buf_offset;
    fwriteInt( fhn, rec_offset );
    fwriteInt( fhn, fragm_len );
    fragm_count++;
        //-- Завершение работы
    fclose( fsrc );
    fclose( filhs);
    fflush( filhs );
    fclose((FILE*)fhn);
    fflush( (FILE*)fhn );
    return  fragm_count;
}
//----------------------------------------------------------

//== 09 sen 2013 ===========================================
//-- work with ref -----------------------------------------

//== создать ссылочный файл на таблицу фрагментов по имени файла
//-- возвращает при успехе 0
//-- или отрицательный код ошибки
int createDBRF( char*  filref_name ) {
        //-- формируем имя ссылок
    char*  fil_dbr = new char[ getLen( filref_name )+ 5];
    filref_name = trim(filref_name, fil_dbr);
    strcat( fil_dbr, ".dbr");
    int     fhr = (int)fopen(fil_dbr, "w");
    if( !fhr ) return  - DBR_NOT_EXIST;  //-- не смогли создать файл dbr
    fclose((FILE*) fhr );
    return  0;
}
//----------------------------------------------------------

//== открыть ссылочный файл на таблицу фрагментов DBSN
//--  возвращает обработчик файла при успехе
//--  или отрицательный код ошибки
int openDBRF( char* filref_name ) {
        //-- формируем имя ссылок
    char*  fil_dbr = new char[ getLen( filref_name )+ 5];
    filref_name = trim(filref_name, fil_dbr);
    strcat( fil_dbr, ".dbr");
    int     fhr = (int)fopen(fil_dbr, "rb+");
    if( !fhr ) return  - DBR_NOT_EXIST;  //-- не смогли создать файл dbr
    return  fhr;
}
//----------------------------------------------------------

//== закрыть ссылочный файл на таблицу фрагментов DBSN
//--  возвращает 0 при успехе
//--  или отрицательный код ошибки
int closeDBRF( int fhr ) {
    if( !fhr ) return  - DBR_NOT_EXIST;  //-- не смогли создать файл dbr
    fclose((FILE*)fhr);
    return  0;
}
//----------------------------------------------------------

//== подсчитать количество записей в файле ссылое
//--  внутренняя без обработки ошибки
int countRefIn( int fhr ) {
    fseek((FILE*)fhr,0,2);
    return ftell( (FILE*)fhr ) /4;
};

//-- внешняя с обработкой ошибок
int countRef( int fhr ) {
    if( !fhr ) return  - DBR_NOT_EXIST;  //-- нет файла dbr
    return  countRefIn( fhr );
};
//----------------------------------------------------------

//== получить номер фрагмента по ссылке на него
//--  возвращает номер фрагмента при успехе
//--  или отрицательный код ошибки
//--  внутренняя без обработки ошибки
int  getRefIn(  int fhr, int ref_nom ){
    fseek((FILE*)fhr,(ref_nom-1)*4,0);
    return   freadInt( fhr );  //-- получили номер фрагмента
}

//-- внешняя с обработкой ошибок
int  getRef(  int fhr, int ref_nom ){
    if( fhr <= 0 ) return -DBR_NOT_EXIST;
    fseek((FILE*)fhr,0,2);
    int     ref_count = ftell( (FILE*)fhr ) /4;
    if(( ref_nom < 1)||(ref_nom > ref_count)) return -OUT_OF_RANGE;
    int     fragm_nom = getRefIn(  fhr, ref_nom );  //-- получили номер фрагмента
    if( fragm_nom < 0 )  fragm_nom *= -1;
    return  fragm_nom;
};
//----------------------------------------------------------

//== получить фрагмент по ссылке на него
//--  возвращает номер ссылки при успехе
//--  или отрицательный код ошибки
int  getFragmByRef(  int dbhadr, int fhr, int ref_nom,  char* fragm, int bufsize ){
    int     fragm_nom = getRef(  fhr, ref_nom );
    if( fragm_nom <= 0 ) return fragm_nom;
    if( getErrCod(dbhadr) >= FATAL_ERROR ) return  - getErrCod(dbhadr);
    setNom( dbhadr, fragm_nom );
    fragm_nom =  getFragm( dbhadr, fragm, bufsize );
    if( fragm_nom < 0 ) return fragm_nom;
    return ref_nom;
};
//----------------------------------------------------------


//== добавить в ссылочный файл номер фрагмента
//--  возвращает номер ссылки при успехе
//--  или отрицательный код ошибки
//--  внутренняя без обработки ошибки
int  addRefIn(  int fhr, int fragm_nom ){
    fseek((FILE*)fhr,0,2);
    fwriteInt( fhr, fragm_nom );
    fseek((FILE*)fhr,0,2);
    return ftell( (FILE*)fhr ) /4;
}

//-- внешняя с обработкой ошибок
int  addRef(  int fhr, int fragm_nom ){
    if( fhr <= 0 ) return -DBR_NOT_EXIST;
    return  addRefIn( fhr, fragm_nom );
}
//----------------------------------------------------------

//== заменить в ссылочном файле номер фрагмента в нужной позиции
//--  возвращает номер ссылки при успехе
//--  или отрицательный код ошибки
//--  внутренняя функция --
int  setRefIn(  int fhr, int ref_nom, int fragm_nom ){
    fseek((FILE*)fhr,(ref_nom-1)*4,0);
    fwriteInt( fhr, fragm_nom );
    return  ref_nom;
}

//-- внешняя с обработкой ошибок
int  setRef(  int fhr, int ref_nom, int fragm_nom ){
    //-- обработка ошибки
    if( fhr <= 0 ) return -DBR_NOT_EXIST;
    if(( ref_nom < 1)||(ref_nom > countRefIn( fhr ))) return -OUT_OF_RANGE;
        //-- записываем в нужное место новое значение
    return  setRefIn(  fhr, ref_nom, fragm_nom );
}
//----------------------------------------------------------

//== удалить в ссылочном файле номер фрагмента в нужной позиции
//--  возвращает номер ссылки при успехе
//--  или отрицательный код ошибки
//--  внутренняя функция --
int  delRefIn(  int fhr, int ref_nom ){
    int     fragm_nom = getRefIn( fhr, ref_nom );  
    if( fragm_nom > 0 ) //-- записываем в нужное место новое значение
        setRefIn(fhr, ref_nom, -fragm_nom);
    return  ref_nom;
}

//-- внешняя с обработкой ошибок
int  delRef(  int fhr, int ref_nom ){
    //-- обработка ошибки
    if( fhr <= 0 ) return -DBR_NOT_EXIST;
    if(( ref_nom < 1)||(ref_nom > countRefIn( fhr ))) return -OUT_OF_RANGE;
    return  delRefIn(  fhr, ref_nom );
}
//----------------------------------------------------------

//== Снять удаление в ссылочном файле
//--  возвращает номер ссылки при успехе
//--  или отрицательный код ошибки
//--  внутренняя функция --
int  undelRefIn(  int fhr, int ref_nom ){
    int     fragm_nom = getRefIn( fhr, ref_nom );
    if( fragm_nom < 0 ) //-- записываем в нужное место новое значение
        setRefIn(fhr, ref_nom, -fragm_nom);
    return  ref_nom;
}

//-- внешняя с обработкой ошибок
int  undelRef(  int fhr, int ref_nom ){
    if( fhr <= 0 ) return -DBR_NOT_EXIST;
    if(( ref_nom < 1)||(ref_nom > countRefIn( fhr ))) return -OUT_OF_RANGE;
    return  delRefIn(  fhr, ref_nom );
}
//----------------------------------------------------------

//== Поменять местами ссылки на фрагменты
//--  возвращает номер первой ссылки при успехе
//--  или отрицательный код ошибки
//--  внутренняя функция --
int  xchgRefIn( int  fhr, int  ref_nom1,  int ref_nom2 ) {
        //-- читаем номера фрагментов
    int     fragm_nom1 = getRefIn( fhr, ref_nom1 );
    int     fragm_nom2 = getRefIn( fhr, ref_nom2 );
        //-- записываем их с перестановкой
    setRefIn( fhr, ref_nom1, fragm_nom2);
    setRefIn( fhr, ref_nom2, fragm_nom1);
    return  ref_nom1;
};

//-- внешняя с обработкой ошибок
int  xchgRef( int  fhr, int  ref_nom1,  int ref_nom2 ) {
        //-- обработка ошибки
    if( fhr <= 0 ) return -DBR_NOT_EXIST;
    int     ref_count = countRefIn( fhr);
    if(( ref_nom1 < 1)||(ref_nom1 > ref_count)) return -OUT_OF_RANGE;
    if(( ref_nom2 < 1)||(ref_nom2 > ref_count)) return -OUT_OF_RANGE;
    return  xchgRefIn( fhr, ref_nom1, ref_nom2 );
};
//----------------------------------------------------------

//==  вырезать физически в ссылочном файле ссылку в нужной позиции
//--  возвращает номер вырезанной или последней ссылки при успехе
//--  или отрицательный код ошибки
//--  пока не заморачиваемся эффективностью
//--  внутренняя функция --
int  cutRefIn(  int fhr, int ref_nom ){
        //-- сдвигаем все дальнейшие влево к началу
    int     frnom_loc,
            refnom_loc,
            ref_count = countRefIn( fhr);
    for( refnom_loc = ref_nom; refnom_loc < ref_count; refnom_loc++) {
        frnom_loc = getRefIn( fhr, refnom_loc+1);
        setRefIn( fhr, refnom_loc, frnom_loc);
    }
        //-- уменьшаем размер файла
    ftruncate(fileno( (FILE*)fhr ), ref_count*4 - 4);
    if( ref_nom < ref_count )
        return  ref_nom;
    return  ref_count - 1;
}


//-- внешняя с обработкой ошибок
int  cutRef(  int fhr, int ref_nom ){
    //-- обработка ошибки
    if( fhr <= 0 ) return -DBR_NOT_EXIST;
    if(( ref_nom < 1)||(ref_nom > countRefIn(fhr))) return -OUT_OF_RANGE;
    return  cutRefIn(fhr, ref_nom );
}
//----------------------------------------------------------

//== вставить в ссылочном файле номер фрагмента в нужной позиции
//--  возвращает номер ссылки при успехе
//--  или отрицательный код ошибки
//--  пока не заморачиваемся эффективностью
//--  внутренняя функция --
int  insRefIn(  int fhr, int ref_nom, int fragm_nom ){
        //-- сдвигаем остаток в сторону конца файла
    int     frnom_loc,
            refnom_loc,
            ref_count = countRefIn( fhr);
    for( refnom_loc = ref_count; refnom_loc >= ref_nom; refnom_loc--) {
        frnom_loc = getRefIn( fhr, refnom_loc);
        setRefIn( fhr, refnom_loc, frnom_loc + 1);
    }
        //-- записываем в нужное место новое значение
    return  setRefIn( fhr, ref_nom, fragm_nom);
}

//-- внешняя с обработкой ошибок
int  insRef(  int fhr, int ref_nom, int fragm_nom ){
    //-- обработка ошибки
    if( fhr <= 0 ) return -DBR_NOT_EXIST;
    if(( ref_nom < 1)||(ref_nom > countRefIn( fhr))) return -OUT_OF_RANGE;
    return  insRefIn( fhr, ref_nom, fragm_nom);
}
//----------------------------------------------------------


//== 02 mar 2014 ===========================================
//-- filters and sorting -----------------------------------


//== очистить фильтр на ссылочном файле
//--  просто делает все номера фрагментов  в ссылочном файле положительными
//--  возвращает число записей в ссылочном файле
//--  или отрицательный код ошибки
int   clearFiltr(  int fhr ){
    if( fhr <= 0 ) return -DBR_NOT_EXIST;
    int     ref_count = countRefIn( fhr );
    for( int ref_nom = 1; ref_nom <= ref_count; ref_nom++ ) {
        undelRefIn( fhr, ref_nom );
    };
    return  ref_count;
};
//----------------------------------------------------------

//== получить смещение подстроки с заданным номером
//-- возвращает смещение или отрицвтельный код ошибки
//-- отрицательный номер подстроки считает от конца строки
int getSubstOffset(char* pc, int nom_sub) {
    if( !pc ) return - BAD_PTR;
    int     subst_count = substCount(pc);
    if( nom_sub <0) nom_sub += subst_count; //-- отриц считаем с конца
    if( (nom_sub <0)||(nom_sub >= subst_count))   return  - BAD_NUMBER;
    char    *pc_tmp = pc;
    while( nom_sub ){
        if( *pc_tmp == '|' ) nom_sub--;
        pc_tmp++;
    }
    return  pc_tmp - pc;
};

//== добавить фильтр на ссылочный файл
//--  проходит по всему ссылочному файлу
//--  для положительных ссылок проверяет условие
//--  и если оно не выполняется делает ссылку отрицательной
//--  возвращает число активных ссылок в ссылочном файле
//--  или отрицательный код ошибки
int  addFiltr(  int dbhadr, int fhr, int subst_nom, char* condition, char* value ){
    if( fhr <= 0 ) return -DBR_NOT_EXIST;
    fseek((FILE*)fhr,0,2);
    int     ref_count = ftell( (FILE*)fhr ) /4;
    int     fragm_nom = 0;
    int     flag_cond = 0;   //-- признак выполнения условия
    int     active_count = 0;  //-- число активных ссылок
    int     res = 0;
    int     buf_size = 4096;
    char*   fragm_buf = new char[buf_size];  //-- буфер для фрагмента
    int     fragm_size = 0;
    int     subst_count = 0;
    int     subst_offset = 0;
    int     subst_len = 0;
    int     isub = 0,   //-- целое значение подстроки
            ival = 0;   //-- целое значение параметра
    char*   pc_subst = 0;
    for( int ref_nom = 1; ref_nom <= ref_count; ref_nom++ ) {
        fragm_nom = getRefIn( fhr, ref_nom );
        if( fragm_nom <= 0 ) continue;  //-- пропускаем пассивные ссылки
            //-- выбираем нужный фрагсмент
        flag_cond = 0;
        res = setNom( dbhadr, fragm_nom );
        if( res < 0 ) return res;
            //-- для активных ссылок проверяем условие
        if( !strcmp( condition, "active" )){
                //-- блокирруем удаленные записи
            if( ! isActive(dbhadr) )  flag_cond = 1;
        } else {  //-- проверка по содержимому фрагмента
                //-- извлекаем фрагмент
            res = getFragmSize( dbhadr, fragm_size );
            if( fragm_size > buf_size ){
                delete[] fragm_buf;
                buf_size = fragm_size + 4096; //-- синдром бита
                fragm_buf = new char[buf_size];
            }
            res = getFragm( dbhadr, fragm_buf, buf_size );
                //-- получаем араметры подстроки
            subst_count = substCount( fragm_buf );
            subst_offset = getSubstOffset( fragm_buf, subst_nom );
            subst_len = substLen( fragm_buf, subst_nom );
            pc_subst = (subst_offset < 0)? 0: fragm_buf + subst_offset;
            res = getSubint( value, ival, 0 );
            res = getSubint( fragm_buf, isub, subst_nom );
                //-- проверка по содержимому фрагмента
            if( !strcmp( condition, "fill" )){
                    //-- блокирруем записи с пустым полем
                 if(( subst_len == 0 )||( subst_count <= subst_nom ))  flag_cond = 1;
            } else if( !strcmp( condition, "str=" )){
                if( !pc_subst || compSpec0(  pc_subst, value ) )  flag_cond = 1;
            } else if ( !strcmp( condition, "str<>" )){
                if( pc_subst && !compSpec0(  pc_subst, value ) )  flag_cond = 1;
            } else if ( !strcmp( condition, "*=*" )){
                if( pc_subst && checkSubst(  pc_subst, value ,0 ) )  flag_cond = 1;
            } else if ( !strcmp( condition, "!*=*" )){
                if( pc_subst && !checkSubst(  pc_subst, value ,0 ) )  flag_cond = 1;
            } else if ( !strcmp( condition, "=" )){
                if( isub != ival ) flag_cond = 1;
            } else if ( !strcmp( condition, "<>" )){
                if( isub == ival ) flag_cond = 1;
            } else if ( !strcmp( condition, ">" )){
                if( isub <= ival ) flag_cond = 1;
            } else if ( !strcmp( condition, "<" )){
                if( isub >= ival ) flag_cond = 1;
            } else if ( !strcmp( condition, ">=" )){
                if( isub < ival ) flag_cond = 1;
            } else if ( !strcmp( condition, "<=" )){
                if( isub > ival ) flag_cond = 1;
            }
        }
            //-- конец проверки условия
        if( flag_cond > 0){
            setRefIn( fhr, ref_nom, -fragm_nom );
        }
        else
            active_count++;
    };
    delete[] fragm_buf;
    return  active_count;
};
//----------------------------------------------------------

//== вытащить целое число из подстроки с заданным номером
//-- возвращает длину подстроки или отриц код ошибки
int   getSubint(char* pc, int& ires, int nom_sub){
    int     subst_offset = getSubstOffset(  pc, nom_sub );
    if(  subst_offset < 0 ) return  subst_offset;
    int     subst_len =  substLen( pc, nom_sub );
    if( subst_len == 0 ) { ires = 0; return 0; }
    int     num = 0;
    int     znak = 0;
    for( ;((*pc<0x30)||(*pc>0x39)); pc++) // find first dec digit
    {   if(*pc==0x2d) znak = 1; //-- look for minus
        if((!*pc)||(*pc == 0x0D)||(*pc == 0x0A)||(*pc == '|')||(!*pc)) return 0;
    }
    for( ;((*pc>=0x30)&&(*pc<=0x39)); pc++ )
        num = num*10 + (*pc-0x30);
    if( znak ) num = -num;
    ires = num;
    return  subst_len;
};
//----------------------------------------------------------


//== сформировать строку в буфере из целого числа
//-- возвращает длину сформированной строки или отриц код ошибки

int  formStint( char* pcbuf, int buf_size, int ival ){
    if( !pcbuf ) return - BAD_PTR;
        //-- отрабатываем нулевое значение
    if( !ival ) { *pcbuf++ = '0';  *pcbuf = 0; return 1; }
        //-- сначала формируем справа налево
    char     znak = ' ';
    if( ival < 0 ){ ival = -ival; znak = '-'; }
    char* pc = pcbuf + buf_size - 1;
    *pc-- = 0;
    while( ival ) {
        if( pc == pcbuf)  return -BUFFER_SMALL;
        *pc-- = (char)(ival%10+0x30);
        ival /= 10;
    }
    *pc = znak;
        //-- переносим строку в левую часть буфера
    int     sval_offset = pc - pcbuf;
    for( int j = 0; j < buf_size - sval_offset; j++ )
        *pcbuf++ = *pc++;
    return  buf_size - sval_offset;
};
//----------------------------------------------------------

//== добавить числовую подстроку к строке сзади
//-- результат записать в буфер pcres
//-- возвращает длину результата или отриц код ошибки
int  addSubint(char* pc, char* pcres, int res_size,  int ival) {
    char    temp_buf[20];
    int     res = formStint( temp_buf, 20, ival );
    if( res < 0 ) return  res;
    return  addSubst( pc, pcres, res_size, temp_buf );
};
//----------------------------------------------------------

//-- вставить числовую подстроку в нужное место строки
//-- результат поместить в буфер pcres
//-- возвращает длину результата или отриц код ошибки
int insSubint(char* pc, char* pcres, int res_size, int nom_sub, int ival) {
    char    temp_buf[20];
    int     res = formStint( temp_buf, 20, ival );
    if( res < 0 ) return  res;
    return  insSubst( pc, pcres, res_size, nom_sub, temp_buf );
};
//----------------------------------------------------------

//-- заменить подстроку на новое числовое значение в нужном месте строки
//-- результат поместить в буфер pcres
//-- возвращает длину результата или отриц код ошибки
int setSubint(char* pc, char* pcres, int res_size, int nom_sub,  int ival ) {
    char    temp_buf[20];
    int     res = formStint( temp_buf, 20, ival );
    if( res < 0 ) return  res;
    return  setSubst( pc, pcres, res_size, nom_sub, temp_buf );
};
//----------------------------------------------------------

//== сформировать ссылочный файл по условию
//--  проходит по всей исходной таблице DBSN
//--  для каждой записи проверяет условие
//--  и если оно выполняется добавляет создает ссылку на эту запись
//--  в ссылочный файл
//--  возвращает число ссылок в ссылочном файле
//--  или отрицательный код ошибки
int  formDBRF(  int dbhadr, int fhr, int subst_nom, char* condition, char* value ){
    if( fhr <= 0 ) return -DBR_NOT_EXIST;
    ftruncate(fileno( (FILE*)fhr ), 0);  //-- обнуляем ссылочный файл
    int     res = countFragmA( dbhadr );
    if( res < 0 ) return res;
    int     fragm_count = res;  //-- число фрагментов в исходной таблице
    int     fragm_nom = 0;
    int     ref_count = 0;  //-- число активных ссылок
    int     buf_size = 4096;
    char*   fragm_buf = new char[buf_size];  //-- буфер для фрагмента
    int     fragm_size = 0;
    int     subst_count = 0;
    int     subst_offset = 0;
    int     subst_len = 0;
    int     isub = 0,   //-- целое значение подстроки
            ival = 0;   //-- целое значение параметра
    char*   pc_subst = 0;
    for(  fragm_nom = 1; fragm_nom <= fragm_count; fragm_nom++ ) {
            //-- игнорируем удаленные записи
        res = setNom( dbhadr, fragm_nom );
        if( res < 0 ) return res;
            //-- общие проверки
        if( !strcmp( condition, "all" )){ //-- выбор всех записей
            addRef(  fhr, fragm_nom );  ref_count++;  continue;
        }
            //-- остальные проверки только для активных записей
        if( !isActive(dbhadr) )  continue;
        if( !strcmp( condition, "active" )){ //-- все активные
            addRef(  fhr, fragm_nom );  ref_count++;  continue;
        }
            //--считываем фрагмент в буфер
        res = getFragmSize( dbhadr, fragm_size );
        if( fragm_size > buf_size ){ //--корректируем размер буфера под фрагмент
            delete[] fragm_buf;
            buf_size = fragm_size + 4096; //-- синдром бита
            fragm_buf = new char[buf_size];
        }
        res = getFragm( dbhadr, fragm_buf, buf_size );
            //-- получаем параметры подстроки
        subst_count = substCount( fragm_buf );
        subst_offset = getSubstOffset( fragm_buf, subst_nom );
        subst_len = substLen( fragm_buf, subst_nom );
        pc_subst = (subst_offset < 0)? 0: fragm_buf + subst_offset;
            //-- проверка по непустому полю
        if( !strcmp( condition, "fill" )){   //-- поле не пусто
            if(( subst_len != 0 )&&( subst_count <= subst_nom )) {
                addRef(  fhr, fragm_nom );  ref_count++;  continue;
            }
        }
            //-- проверка на вхождение строки в фрагмент
        if( !strcmp( condition, "*=*" )){
            if( pc_subst && checkSubst(  pc_subst, value, 0 ) ){
                addRef(  fhr, fragm_nom );  ref_count++;  continue;
            }
        }
            //-- проверка на невхождение строки в фрагмент
        if( !strcmp( condition, "!*=*" )){
            if( pc_subst && !checkSubst(  pc_subst, value, 0 ) ){
                addRef(  fhr, fragm_nom );  ref_count++;  continue;
            }
        }
            //-- проверка на равенство фрагмента строке
        if( !strcmp( condition, "str=" )){
            if( pc_subst && !compSpec0(  pc_subst, value ) ){
                addRef(  fhr, fragm_nom );  ref_count++;  continue;
            }
        }
            //-- проверка на неравенство фрагмента строке
        if( !strcmp( condition, "str<>" )){
            if( pc_subst && compSpec0(  pc_subst, value ) ){
                addRef(  fhr, fragm_nom );  ref_count++;  continue;
            }
        }
        res = getSubint( value, ival, 0 );
        res = getSubint( fragm_buf, isub, subst_nom );
            //-- проверка на числовое равенство фрагмента строке
        if( !strcmp( condition, "=" )){
            if( isub == ival ){
                addRef(  fhr, fragm_nom );  ref_count++;  continue;
            }
        }
            //-- проверка на числовое неравенство фрагмента строке
        if( !strcmp( condition, "!=" )){
            if( isub != ival ){
                addRef(  fhr, fragm_nom );  ref_count++;  continue;
            }
        }
            //-- проверка на числовое больше
        if( !strcmp( condition, ">" )){
            if( isub > ival ){
                addRef(  fhr, fragm_nom );  ref_count++;  continue;
            }
        }
            //-- проверка на числовое меньше
        if( !strcmp( condition, "<" )){
            if( isub < ival ){
                addRef(  fhr, fragm_nom );  ref_count++;  continue;
            }
        }
            //-- проверка на числовое больше или равно
        if( !strcmp( condition, ">=" )){
            if( isub >= ival ){
                addRef(  fhr, fragm_nom );  ref_count++;  continue;
            }
        }
            //-- проверка на числовое меньше или равно
        if( !strcmp( condition, "<=" )){
            if( isub <= ival ){
                addRef(  fhr, fragm_nom );  ref_count++;  continue;
            }
        }
    };
    delete[] fragm_buf;
    return  ref_count;
};
//----------------------------------------------------------

//== вспомогательная функция сравнения подстрок
//-- сравнивает между собой заданную подстроку двух строк
//-- на основании условия condition
//-- регистр игнорирует, возвращает
//-- 1  если условие выполняется
//-- 0  если не выполняется
//-- при сравнении границу подстрок "|" считает меньше любого симв
//-- 

int   compSubst(  char* pc1, char* pc2, int subst_nom, char* condition ) {
    int     subst1_offset = getSubstOffset( pc1, subst_nom );
    char*   pc_subst1 = pc1 + subst1_offset;
    int     subst2_offset = getSubstOffset( pc2, subst_nom );
    char*   pc_subst2 = pc2 + subst2_offset;
    if( !strcmp( condition, "str>" )){
        if( compSpec0(  pc_subst1, pc_subst2 ) > 0 )  return 1;
        return 0;
    }
    if( !strcmp( condition, "str>=" )){
        if( compSpec0(  pc_subst1, pc_subst2 ) >= 0 )  return 1;
        return 0;
    }
    if( !strcmp( condition, "str<" )){
        if( compSpec0(  pc_subst1, pc_subst2 ) < 0 )  return 1;
        return 0;
    }
    if( !strcmp( condition, "str<=" )){
        if( compSpec0(  pc_subst1, pc_subst2 ) <= 0 )  return 1;
        return 0;
    }
        //-- блок числового сравнения
    int     res = 0; 
    int     isub1 =0, isub2=0;
    res = getSubint( pc1, isub1, subst_nom );
    res = getSubint( pc2, isub2, subst_nom );
    if( !strcmp( condition, ">" )){
        if( isub1 > isub2 )  return 1;
        return 0;
    }
    if( !strcmp( condition, ">=" )){
        if( isub1 >= isub2 )  return 1;
        return 0;
    }
    if( !strcmp( condition, "<" )){
        if( isub1 < isub2 )  return 1;
        return 0;
    }
    if( !strcmp( condition, "<=" )){
        if( isub1 <= isub2 )  return 1;
        return 0;
    }
    return  res;  //-- по умолчанию выполяется
}


//== отсортировать ссылочный файл по условию
//--  проходит по всей исходной таблице DBSN
//--  для каждой пары записей проверяет условие
//--  возвращает число активных записей в ссылочном файле
//--  или отрицательный код ошибки
int  sortDBRF(  int dbhadr, int fhr, int subst_nom, char* condition ){
    if( fhr <= 0 ) return -DBR_NOT_EXIST;
    int     res = countFragmA( dbhadr );
    if( res < 0 ) return res;
    //int     fragm_count = res;  //-- число фрагментов в исходной таблице
    int     fragm1_nom = 0;
    int     fragm2_nom = 0;
    int     temp_nom = 0;
    int     ref_count = countRef( fhr );  //-- число  ссылок
    int     del_count = 0;
    int     ref1_nom = 0;
    int     ref2_nom = 0;
    int     reftemp_nom = 0;
        //-- сдвигаем в конец удаленные записи
    for( ref1_nom = 1, ref2_nom = ref_count; ref1_nom < ref2_nom; ref1_nom++){
        fragm1_nom = getRef(  fhr, ref1_nom );
        if( fragm1_nom < 0 ){
                //-- фрагмент удален ищем место в конце
            for( del_count++ ; ref2_nom > ref1_nom; ref2_nom--){
                fragm2_nom = getRefIn(  fhr, ref2_nom );
                if( fragm2_nom > 0 ) break;
            }
            if( fragm2_nom > 0 ) xchgRef( fhr, ref1_nom,  ref2_nom );
        }
    }
    ref_count -= del_count;
    if( ref_count <= 0 ) return 0;  //--выходим если нет активных записей
    int     flag_sort = 0;   //-- признак необходимости сортировки
        //-- проверка на сортировку по номерам фрагментов
    if( !strcmp( condition, "nom+" )){ //-- по возрастанию номеров
            //-- проверяем необходимость
        for( ref1_nom = 1; ref1_nom < ref_count; ref1_nom++){
            fragm1_nom = getRef(  fhr, ref1_nom );
            fragm2_nom = getRef(  fhr, ref1_nom + 1 );
            if( fragm2_nom < fragm1_nom ){ flag_sort = 1; break;}
        }
        if( !flag_sort )  return ref_count;
            //--сортируем, т.к. сортировка необходима
        for( ref2_nom = ref_count; ref2_nom >1; ref2_nom--){
            fragm2_nom = getRef(  fhr, ref2_nom );
            temp_nom = fragm2_nom;  reftemp_nom = ref2_nom;
            for( ref1_nom = 1; ref1_nom < ref2_nom; ref1_nom++){
                    //-- ищем максимальный
                fragm1_nom = getRef(  fhr, ref1_nom );
                if( fragm1_nom > temp_nom ) { temp_nom = fragm1_nom;  reftemp_nom = ref1_nom; }
            }
                //--если надо выполняем перестановку
            if(  temp_nom > fragm2_nom )
                xchgRef( fhr, ref2_nom,  reftemp_nom );
        }
        return ref_count;
    }
    if( !strcmp( condition, "nom-" )){ //-- по убыванию номеров
            //-- проверяем необходимость
        for( ref1_nom = 1; ref1_nom < ref_count; ref1_nom++){
            fragm1_nom = getRef(  fhr, ref1_nom );
            fragm2_nom = getRef(  fhr, ref1_nom + 1 );
            if( fragm2_nom > fragm1_nom ){ flag_sort = 1; break;}
        }
        if( !flag_sort )  return ref_count;
            //--сортируем, т.к. сортировка необходима
        for( ref2_nom = ref_count; ref2_nom >1; ref2_nom--){
            fragm2_nom = getRef(  fhr, ref2_nom );
            temp_nom = fragm2_nom;  reftemp_nom = ref2_nom;
            for( ref1_nom = 1; ref1_nom < ref2_nom; ref1_nom++){
                    //-- ищем минимальный
                fragm1_nom = getRef(  fhr, ref1_nom );
                if( fragm1_nom < temp_nom ) { temp_nom = fragm1_nom;  reftemp_nom = ref1_nom; }
            }
                //--если надо выполняем перестановку
            if(  temp_nom < fragm2_nom )
                xchgRef( fhr, ref2_nom,  reftemp_nom );
        }
        return ref_count;
    }
    //== блок сортировки по седержимому фрагментов
        //--сначала находим размер буфера по максимальному фрагменту
    int     buf_size = 0, fragm_size = 0;
    for( ref1_nom = 1; ref1_nom <= ref_count; ref1_nom++){
        fragm1_nom = getRef(  fhr, ref1_nom );
        setNom( dbhadr, fragm1_nom );
        getFragmSize( dbhadr, fragm_size );
        if( fragm_size > buf_size ) buf_size = fragm_size;
    }
    buf_size += 16;  //-- запас на синдром бита
    char*   fragm1_buf = new char[buf_size];  //-- буфер для 1-го фрагмента
    char*   fragm2_buf = new char[buf_size];  //-- буфер для 2-го фрагмента
        //-- основной цикл сравнения
    for( ref2_nom = ref_count; ref2_nom >1; ref2_nom--){
        fragm2_nom = getRef(  fhr, ref2_nom );
        getFragmByRef( dbhadr, fhr, ref2_nom, fragm2_buf, buf_size );
        temp_nom = fragm2_nom;  reftemp_nom = ref2_nom;
        for( ref1_nom = 1; ref1_nom < ref2_nom; ref1_nom++){
                //-- ищем минимальный
            fragm1_nom = getRef(  fhr, ref1_nom );
            getFragmByRef( dbhadr, fhr, ref1_nom, fragm1_buf, buf_size );
            if( compSubst(  fragm1_buf, fragm2_buf, subst_nom, condition )){
                temp_nom = fragm1_nom;  reftemp_nom = ref1_nom;
                strcpy( fragm1_buf, fragm2_buf);
            }
        }
            //--если надо выполняем перестановку
        if(  temp_nom < fragm2_nom )
            xchgRef( fhr, ref2_nom,  reftemp_nom );
    }
    delete[] fragm1_buf;
    delete[] fragm2_buf;
    return  ref_count;
};
//----------------------------------------------------------

//== проверить, содержит ли подстрока subst_nom значение templ
//-- возвращает 1 если templ имеется
//-- или 0 если не нашла templ в подстроке
int   checkSubst(  char* pc, char* templ, int subst_nom ) {
    int     subst_offset = getSubstOffset( pc, subst_nom );
    char*   pc_subst = pc + subst_offset;
    int     subst_len = substLen( pc, subst_nom );
    int     templ_len = getLen( templ);
    char*   pc_loc;
    int     j=0;
        //-- переводим шаблон в верхний регистр
    for( j = 0; j < templ_len; j++) {
        templ[j] =  toupper( templ[j] );
    }
        //--  проверяем по всем возможным позициям
    for( int pos = 0;  pos <= subst_len - templ_len;  pos++) {
        pc_loc = pc_subst+pos;
        for( j = 0; j< templ_len; j++ ){
            if( toupper(pc_loc[j]) != templ[j] )  break;
        }
        if( j == templ_len )
            return  1;   //-- нашли совпадение
    }
    return 0;
}

//==========================================================
//== 17 mar 2014 ==
//== скопировать файл ссылок под другим именем
//--  возвращает количество скопированых записей
//--  или 0 при ошибке
//-- при  flag_pack != 0 удаленные записи не копируются

int    copyDBRF( char*  fil_ref, char*  copy_ref, int flag_pack ) {
        //-- открываем исходный и результирующий файлы
    int     fhr = openDBRF( fil_ref );
    if( fhr < 0 ) return  fhr;
    int     res = createDBRF( copy_ref );
    if( res < 0 ) return res;
    int     new_fhr = openDBRF( copy_ref );
    if( new_fhr < 0 ) return new_fhr;
        //-- копируем в новый файл ссылки
    fseek((FILE*)fhr,0,2);
    int     ref_count = ftell( (FILE*)fhr ) /4;
    int     fragm_nom = 0;
    for( int ref_nom = 1; ref_nom <= ref_count; ref_nom++ ) {
        fragm_nom = getRefIn( fhr, ref_nom );
        if( !flag_pack ) //-- копировать все записи
            addRefIn(  new_fhr, fragm_nom );
        else {
            if( fragm_nom > 0 )
                addRefIn(  new_fhr, fragm_nom );
        }
    };
        //-- закрываем файлы ссылок
    res = closeDBRF( fhr );
    ref_count = countRefIn( new_fhr );
    res = closeDBRF( new_fhr );
    if( res < 0 ) return res;
    return  ref_count;
};

//== упаковать файл ссылок
//--  оставляет только записи с положительными номерами фрагментов
//--  возвращает число записей в упакованом файле
int  packDBRF(  int fhr  ){
        //-- подготавливаем переменные
    if( fhr <= 0 ) return -DBR_NOT_EXIST;
    int     new_refnom = 0;
    int     ref_count = ftell( (FILE*)fhr ) /4;
    int     fragm_nom = 0;
    for( int ref_nom = 1; ref_nom <= ref_count; ref_nom++ ) {
        fragm_nom = getRefIn( fhr, ref_nom );
        if( fragm_nom > 0 ){
            new_refnom++;
            setRefIn(  fhr, new_refnom, fragm_nom );
        }
    };
        //-- уменьшаем размер файла ссылок и закрываем  его
    ftruncate(fileno( (FILE*)fhr ), new_refnom*4 );
    ref_count = ftell( (FILE*) fhr ) /4;
    int     res = closeDBRF( fhr );
    if( res < 0 ) return res;
    return  ref_count;
};
//----------------------------------------------------------

//== сформировать новую таблицу как выботку по ссылкам
//--  создает новую таблицу,
//--  проходит по всему ссылочному файлу DBRF
//--  и копирует в новую таблицу каждый активный фрагмент исходного DBSN
//--  возвращает число записей в новой таблице
//--  или отрицательный код ошибки
int  selectDBSN(  int dbhadr, int fhr, char* new_tabl ){
        //-- начальные проверки
    if( !dbhadr )   return 0;
    int*    dbh = ( int* )dbhadr;
    if( dbh[3] > FATAL_ERROR)    return  -dbh[3];
    dbh[3] = 0; //-- стираем предупреждения
    if( fhr <= 0 ) return -DBR_NOT_EXIST;
        //-- создаем новую таблицу
    int     res = createDBSN( new_tabl);
    if( res < 0 ) return res;
    res = openDBSNA( new_tabl );
    int     new_dbhadr = res;
    //== расчитываем длину буфера фрагмента
    int     ref_nom,
            ref_count =  countRef( fhr );
    int     fr_nom = 0,
            fr_count = 0;
    int     fr_size = 0,
            max_frsize = 0,
            buf_size = 4096;
        //-- проходим по всем ссылкам
    for(  ref_nom = 1; ref_nom <= ref_count; ref_nom++ ) {
        fr_nom = getRefIn( fhr, ref_nom );
        if( fr_nom > 0 ){ //-- только для активных ссылок
            setNom( dbhadr, fr_nom );
            getFragmSize( dbhadr, fr_size );
            if( fr_size > max_frsize )  max_frsize = fr_size ;
            fr_count++;
        }
    }
    if( !fr_count ) { //-- нет активных ссылок
        close( new_dbhadr );
        return 0;
    }
        //-- формируем буфер фрагмента
    if( buf_size <= max_frsize ) buf_size = max_frsize + 16;
    char    *fragm;
    fragm   = new char[buf_size];
    //=== копируем в новую таблицу все активные фрагменты
    for(  fr_count=0, ref_nom = 1; ref_nom <= ref_count; ref_nom++ ) {
        fr_nom = getRefIn( fhr, ref_nom );
        if( fr_nom > 0 ){ //-- только для активных ссылок
            setNom( dbhadr, fr_nom );
            getFragmSize( dbhadr, fr_size );
            if( fr_size > 0 ) {
               res = getFragm( dbhadr, fragm, buf_size );
               addFragm( new_dbhadr, fragm );
               fr_count ++;
            }
        }
    }
    closeDBSN( new_dbhadr );
    return  fr_count;
};
//----------------------------------------------------------

//== вставить в ссылочном файле номер фрагмента после заданной позиции
//--  возвращает номер новой ссылки при успехе
//--  или отрицательный код ошибки
//--  пока не заморачиваемся эффективностью
int  insRefAfter(  int fhr, int ref_nom, int fragm_nom ){
    //-- обработка ошибки
    if( fhr <= 0 ) return -DBR_NOT_EXIST;
    fseek((FILE*)fhr,0,2);
    int     ref_count = ftell( (FILE*)fhr ) /4;
    if(( ref_nom < 1)||(ref_nom > ref_count)) return -OUT_OF_RANGE;
    if( ref_nom == ref_count )
        return  addRef(  fhr, fragm_nom );
        //-- сдвигаем остаток в сторону конца файла
    int     frnom_loc;
    int     refnom_loc;
    for( refnom_loc = ref_count; refnom_loc > ref_nom; refnom_loc--) {
        frnom_loc = getRefIn( fhr, refnom_loc );
        setRefIn( fhr, refnom_loc + 1, frnom_loc );
    }
        //-- записываем в нужное место новое значение
    setRefIn( fhr, ref_nom+1, fragm_nom );
    return  ref_nom + 1; //-- номер записи со вставленой ссылкой
}
//----------------------------------------------------------

//== переместить ссылку ref_nom2 на место ref_nom1 со сдвигом
//--  возвращает номер новой ссылки ( ref_nom1 ) при успехе
//--  или отрицательный код ошибки
//--  пока не заморачиваемся эффективностью
int  moveRef(  int fhr, int ref_nom1, int ref_nom2 ){
    //-- обработка ошибки
    if( fhr <= 0 ) return -DBR_NOT_EXIST;
    int     ref_count = countRefIn( fhr );
    if(( ref_nom1 < 1)||(ref_nom1 > ref_count)) return -OUT_OF_RANGE;
    if(( ref_nom1 < 2)||(ref_nom2 > ref_count)) return -OUT_OF_RANGE;
    if(ref_nom1 == ref_nom2 ) return ref_nom1; //--тривиальная

    int     frnom_tmp = getRefIn( fhr, ref_nom1 );  //-- запомнили что сдвигаем
    int     frnom_loc;
    int     refnom_loc;

    if(ref_nom1 < ref_nom2 ){  //-- надо сдвинуть вперед
        for( refnom_loc = ref_nom2; refnom_loc > ref_nom1; refnom_loc--) {
            frnom_loc = getRefIn( fhr, refnom_loc-1 );
            setRefIn( fhr, refnom_loc, frnom_loc);
        }
    } else { //-- надо сдвинуть назад
        for( refnom_loc = ref_nom2; refnom_loc < ref_nom1; refnom_loc++) {
            frnom_loc = getRefIn( fhr, refnom_loc+1 );
            setRefIn( fhr, refnom_loc, frnom_loc);
        }
    }
    setRefIn( fhr, ref_nom1, frnom_tmp );
    return ref_nom1;
}
//----------------------------------------------------------


//== вдавить блок ссылок на следующий уровень
//--  блок из ref_count ссылок начиная с ref_nom включительно
//--  обрамляет скобками 0х80000000, 0х00000000
//--  что интерпретируется как вложенный блок(напр - подменю)
//--  возвращает номер ссылки на начальную скобку ( ref_nom ) при успехе
//--  или отрицательный код ошибки
//--  пока не заморачиваемся эффективностью
int  pushRblock(  int fhr, int ref_nom, int block_len ){
    //-- обработка ошибок
    if( fhr <= 0 ) return -DBR_NOT_EXIST;
    if(( ref_nom < 1)||(block_len < 0 )||(ref_nom + block_len > countRefIn( fhr ))) return -OUT_OF_RANGE;
    insRefIn( fhr, ref_nom + block_len, END_BRACKET );
    insRefIn( fhr, ref_nom, BEG_BRACKET);
    return  ref_nom;
}
//----------------------------------------------------------

//== найти начало блока( открывающую скобку )
//-- возвращает номер ссылки на открывающую скобку
int  findRblock(  int fhr, int ref_nom ){
    if( fhr <= 0 ) return -DBR_NOT_EXIST;
    int     ref_count = countRefIn( fhr );
    if(( ref_nom < 1)||(ref_nom > ref_count )) return -OUT_OF_RANGE;
    while( BEG_BRACKET != getRefIn( fhr, ref_nom )){
        ref_nom++;
        if( ref_nom > ref_count ) return -OUT_OF_RANGE;
    }
    return ref_nom;
}

//== найти соответствующую закрывающую скобку
//-- возвращает номер ссылки на соответствующую закрывающую скобку
int  endRblock(  int fhr, int ref_nom ){
    if( fhr <= 0 ) return -DBR_NOT_EXIST;
    int     ref_count = countRefIn( fhr );
    if(( ref_nom < 1)||(ref_nom > ref_count )) return -OUT_OF_RANGE;
    if( BEG_BRACKET != getRefIn( fhr, ref_nom ))  return - NOT_A_BRACKET;
    int     bracket_count = 1;  //-- число скобок
        //-- ишем соответствующую закрывающую скобку
    for( ref_nom++; ref_nom <= ref_count; ref_nom++){
        if( BEG_BRACKET == getRefIn( fhr, ref_nom ))
            bracket_count++;
        if( END_BRACKET == getRefIn( fhr, ref_nom ))
            bracket_count--;
        if( !bracket_count)  break;
    }
    if(ref_nom > ref_count ) return -OUT_OF_RANGE;
    return  ref_nom;
}

//== найти соответствующую открывающую скобку (незад)
//-- возвращает номер ссылки на соответствующую закрывающую скобку
int  begRblock(  int fhr, int ref_nom ){
    if( fhr <= 0 ) return -DBR_NOT_EXIST;
    int     ref_count = countRefIn( fhr );
    if(( ref_nom < 1)||(ref_nom > ref_count )) return -OUT_OF_RANGE;
    if( END_BRACKET != getRefIn( fhr, ref_nom ))  return - NOT_A_BRACKET;
    int     bracket_count = 1;  //-- число скобок
        //-- ишем соответствующую закрывающую скобку
    for( ; ref_nom >=1; ref_nom--){
        if( END_BRACKET == getRefIn( fhr, ref_nom ))
            bracket_count++;
        if( BEG_BRACKET == getRefIn( fhr, ref_nom ))
            bracket_count--;
        if( !bracket_count)  break;
    }
    if(ref_nom <1 ) return -OUT_OF_RANGE;
    return  ref_nom;
}

//== вынуть блок ссылок на предидущий уровень
//--  в исходном состоянии ref_nom установлено на BEG_BRACKET
//--  если это не так - возвращает - NOT_A_BRACKET
//--  иначе ищет соответствующую ей завершающую скобку
//--  и вырезает из файла ссылок эти две записи
//--  что интерпретируется как поднятие вложенного блока(напр - подменю)
//--  на уровень выше
//--  возвращает номер ссылки на бывшую начальную скобку ( ref_nom ) при успехе
//--  или отрицательный код ошибки
int  popRblock(  int fhr, int ref_nom ){
        //-- обработка ошибок
    int     end_nom = endRblock( fhr, ref_nom );
    if( end_nom < 0 ) return end_nom;
        //-- удаляем завершающую и начальную скобки
    cutRefIn( fhr, end_nom );
    return  cutRefIn( fhr, ref_nom );
}
//----------------------------------------------------------

//== вырезать блок из файла ссылок
//--  блок из block_len ссылок начиная с ref_nom включительно
//--  возвращает ref_nom или ref_nom-1  если это был хвост файла
//--  или отрицательный код ошибки
int  cutRblock(  int fhr, int ref_nom, int block_len ){
        //-- обработка ошибок
    if( fhr <= 0 ) return -DBR_NOT_EXIST;
    int     ref_count = countRefIn( fhr );
    if(( ref_nom < 1)||(ref_count < 0 )||(ref_nom + block_len > ref_count )) return -OUT_OF_RANGE;
        //-- обработать хвост
    if( ref_nom + block_len == ref_count ) {
        ref_nom--;  //--  просто уменьшаем размер файла ссылок
        ftruncate(fileno( (FILE*)fhr ), ref_nom*4 );
        return  ref_nom;
    }
        //-- вырезать середину
    int     fragm_nom;
    for( int ref_tmp = ref_nom; ref_tmp < ref_count - block_len; ref_nom++) {
        fragm_nom = getRefIn( fhr, ref_tmp + block_len);
        setRefIn( fhr, ref_tmp, fragm_nom );
    }
    return  ref_nom;
}
//----------------------------------------------------------

//== переместить блок ссылок в новую позицию файла
//--  блок из block_len ссылок начиная с ref_nom включительно
//--  возвращает ref_nom или ref_nom-1  если это был хвост файла
//--  или отрицательный код ошибки
int  moveRblock(  int fhr, int new_nom, int ref_nom, int block_len ){
        //-- обработка ошибок
    if( fhr <= 0 ) return -DBR_NOT_EXIST;
    int     ref_count = countRefIn( fhr );
    if(( ref_nom < 1)||(ref_count < 0 )||(ref_nom + block_len > ref_count )) return -OUT_OF_RANGE;
    if(( new_nom < 1)||(new_nom + block_len > ref_count )) return -OUT_OF_RANGE;
    if( new_nom == ref_nom ) return  new_nom; //-- тривиальный случай
        //-- нельзя смещать блок с пересечением
    if(( new_nom > ref_nom )&&(new_nom < ref_nom + block_len )) return -OUT_OF_RANGE;
        //-- запомнить сдвигаемый блок
    int     nom;
    int*    fragm_arr = new int[ block_len];
    for( nom = 0; nom < block_len; nom++)
        fragm_arr[ nom ] = getRefIn( fhr, ref_nom + nom);
        //-- обработать сдвиг влево
    int     ref_tmp;
    int     fragm_nom;
    if( new_nom < ref_nom  ) {
            //-- сдвинуть вправо
        for( ref_tmp = ref_nom-1; ref_tmp >= new_nom; ref_tmp-- ) {
            fragm_nom = getRefIn( fhr, ref_tmp );
            setRefIn( fhr, ref_tmp+block_len, fragm_nom );
        }
    } else {  //-- обработать сдвиг влево
        for( ref_tmp = ref_nom + block_len; ref_tmp < new_nom + block_len; ref_tmp++ ) {
            fragm_nom = getRefIn( fhr, ref_tmp );
            setRefIn( fhr, ref_tmp-block_len, fragm_nom );
        }
    }
       //-- вставляем запомненный блок
    for( nom = 0 ; nom < block_len; nom++ ){
        fragm_nom = fragm_arr[ nom ];
        setRefIn( fhr, new_nom + nom, fragm_nom );
    }
    delete[] fragm_arr;
    return  new_nom;   
}
//----------------------------------------------------------

//== добавить блок ссылок из другого файла
//--  блок из block_len ссылок  из fhr2 начиная с ref_nom2 включительно
//--  добавляется в конец исходного файла ссылок
//--  возвращает новое чмсло записей в fhr
//--  или отрицательный код ошибки
int  addRblock(  int fhr, int fhr2, int ref_nom2, int block_len ){
        //-- обработка ошибок
    if(( fhr <= 0 )||( fhr2 <= 0 )) return -DBR_NOT_EXIST;
    int     ref_count2 = countRefIn( fhr2 );
    if(( ref_nom2 < 1)||(block_len < 1 )||(ref_nom2 + block_len > ref_count2 )) return -OUT_OF_RANGE;
        //-- добавляем записи
    int     fragm_nom;
    for(int nom = 0; nom < block_len; nom++ ){
        fragm_nom = getRefIn( fhr2, ref_nom2 + nom);
        addRefIn( fhr, fragm_nom);
    }
    return  countRefIn( fhr);
}
//----------------------------------------------------------

//== вставить блок ссылок из другого файла
//--  блок из block_len ссылок  из fhr2 начиная с ref_nom2 включительно
//--  вставляется в нужное место исходного файла ссылок
//--  возвращает новое чмсло записей в fhr
//--  или отрицательный код ошибки
int  insRblock(  int fhr, int ref_nom,int fhr2, int ref_nom2, int block_len ){
        //-- обработка ошибок
    if(( fhr <= 0 )||( fhr2 <= 0 )) return -DBR_NOT_EXIST;
    int     ref_count = countRefIn( fhr );
    if(( ref_nom < 1)||(ref_nom > ref_count )) return -OUT_OF_RANGE;
    int     ref_count2 = countRefIn( fhr2 );
    if(( ref_nom2 < 1)||(block_len < 1 )||(ref_nom2 + block_len > ref_count2 )) return -OUT_OF_RANGE;
        //-- добавляем записи
    int     fragm_nom;
    for(int nom = 0; nom < block_len; nom++ ){
        fragm_nom = getRefIn( fhr2, ref_nom2 + nom);
        insRefIn( fhr, ref_nom + nom, fragm_nom);
    }
    return  ref_nom;
}
//----------------------------------------------------------


//==  Записать текст в нужную запись базы DBSN
//--  возвращает номер записи
//--  или -код фатальной ошибки
//--  или 0 при записи вне границ
int  writeFragm(int dbhadr, char* fragm, int fragm_nom ) {
    if( getErrCod(dbhadr) >= FATAL_ERROR ) return  - getErrCod(dbhadr);
    int  res = setNomIn( (int *) dbhadr, fragm_nom );
    if( res < 0 )  return res;
    return  setFragm(  dbhadr, fragm );
}
//----------------------------------------------------------


//== Считать текущий фрагмент базы DBSN  ===================
//--  возвращает номер записи или
//--  или отрицвтельный код ошибки
//--  или 0 для пустой записи

//-- внутренняя функция (если нет фатальных ошибок)
int  readFragm( int dbhadr, char* fragm, int bufsize, int fragm_nom ) {
    if( getErrCod(dbhadr) >= FATAL_ERROR ) return  - getErrCod(dbhadr);
    int  res = setNomIn( (int *) dbhadr, fragm_nom );
    if( res < 0 )  return res;
    return  getFragm( dbhadr, fragm, bufsize );
};
//----------------------------------------------------------

//==  Сосчитать число активных записей в таблице DBSN
//--  возвращает общее количество активных записей
//--  или отрицвтельный код фатальной ошибки

//-- внутренняя функция (если нет фатальных ошибок)
int countFragmIn( int* dbh ) {
    dbh[3] = 0; //-- стираем предупреждения
    //-- вытаскиваем обработчики файлов
    int     fhn = dbh[0];
    fseek((FILE*)fhn,0,2);
    int     rec_count = ftell( (FILE*)fhn ) /8;
    if( !rec_count )
        dbh[3] = DBN_IS_EMPTY;

    int     active_count = 0,  rec_size;
    for( int rec_nom = 1; rec_nom <= rec_count; rec_nom++) {
    //-- считываем координаты строчного фрагмента
        fseek( (FILE*)fhn, (  rec_nom-1)*8+4, 0);
        rec_size = freadInt(   fhn );
        if(  rec_size > 0) active_count++;
    }
    return  active_count;
}

//-- экпортируемый вариант ---
int countFragm( int dbhadr ) {
    if( getErrCod(dbhadr) >= FATAL_ERROR ) return  - getErrCod(dbhadr);
    return  countFragmIn( (int*) dbhadr );

};
//----------------------------------------------------------

//== открыть таблицу фрагментов DBSN
//--  возвращает обработчик таблицы при успехе
//--  (адрес массива параметров )
//--  и устанавливает номер записи на первый активный
//--  или отрицательный код ошибки
int openDBSN( char* fil_name ) {
    int     res = openDBSNA( fil_name );
    if( res < 0 ) return  res;
    int     dbhadr = res;
    res = gotoFirst( dbhadr );
    if( res < 0 ) return  res;
    return  dbhadr;
}
//----------------------------------------------------------

//==  перейти вперед к следующему активному фрагменту
//--    возвращает номер фрагмента
//--    или -код фатальной ошибки
//--    или 0 при невозможности ( нет столько активных записей )
int    gotoNext( int dbhadr ){
    return  goForward( dbhadr, 1);
}
//----------------------------------------------------------

//==  перейти назад к предидущему активному фрагменту
//--    возвращает номер фрагмента
//--    или -код фатальной ошибки
//--    или 0 при невозможности ( нет столько активных записей )
int    gotoPrev( int dbhadr ){
    return  goBack( dbhadr, 1);
}
//----------------------------------------------------------

//-- сравнить нулевые подстроки 2х строк с игнорирование регистра
//-- 1  если subst(pc,0) > subst(pc1)
//-- 0  если они равны
//-- -1 если subst(pc,0) < subst(pc1)
//-- останавливается по 0 или "|" или любой из строк
long    compSpec0( char* pc, char* pc1){
    unsigned  char  bt, bt1;
    while( 1 ){
        bt = *pc++;  bt1 = *pc1++;
        //-- отрабатываем концы строк
        if((( bt == 0 )||( bt == '|'))&&(( bt1 == 0 )||( bt1 == '|')))  return 0;
        if(( bt == 0 )||( bt == '|'))    return -1;
        if(( bt1 == 0 )||( bt1 == '|'))  return  1;
        //-- равниваем без учета регистра
        bt = toupper(bt);  bt1 = toupper(bt1);
        if( bt == bt1 )  continue;
        if( bt > bt1 )   return  1;
        if( bt < bt1 )   return -1;
    }
};
//----------------------------------------------------------

////////////////////////////////////////////////////////////
/*  далее функции, которые возвращают
      int > 0  при правильном вып
      int = 0  при замечании
      int < 0  при ошибке
*/
//== Считать максимальную длину фрагмента базы DBSN
int    getMaxFragmSize( int  dbhadr ){
    int     max_size = 0;
    int     rec_size = 0;
    int     id = gotoFirst( dbhadr );
    while( id > 0 ) {
        getFragmSize( dbhadr, rec_size );
        if( rec_size > max_size )
            max_size = rec_size;
        id = gotoNext( dbhadr );
    }
    return  max_size;
}
//----------------------------------------------------------

//== Прочитать следующий фрагмент базы DBSN
int     getNextFragm(  int dbhadr, char* fragm, int bufsize ){
    int     id = gotoNext( dbhadr );
    if( id > 0 ){
        return  getFragm(  dbhadr, fragm, bufsize );
    }
    *fragm = 0;
    return  id;
}
//----------------------------------------------------------

//== Прочитать предыдующий фрагмент базы DBSN
int     getPrevFragm(  int dbhadr, char* fragm, int bufsize ){
    int     id = gotoPrev( dbhadr );
    if( id > 0 ){
        return  getFragm(  dbhadr, fragm, bufsize );
    }
    *fragm = 0;
    return  id;
}
//----------------------------------------------------------

//== Прочитать первый фрагмент базы DBSN
int     getFirstFragm(  int dbhadr, char* fragm, int bufsize ){
    int     id = gotoFirst( dbhadr );
    if( id > 0 ){
        return  getFragm(  dbhadr, fragm, bufsize );
    }
    *fragm = 0;
    return  id;
}
//----------------------------------------------------------

//== Прочитать последний фрагмент базы DBSN
int     getLastFragm(  int dbhadr, char* fragm, int bufsize ){
    int     id = gotoLast( dbhadr );
    if( id > 0 ){
        return  getFragm(  dbhadr, fragm, bufsize );
    }
    *fragm = 0;
    return  id;
}
//----------------------------------------------------------

//-- удалить все  пустые подстроки справа
//-- возвращает длину получившейся строки или отриц код ошибки
int cutRightEmpty(char* pc ) {    
        //-- выполняем проверки
    if( !pc ) return - BAD_PTR;    
    int     st_len = getLen( pc);
    if( st_len <1 )  return 0;
    char    *pc_src = pc + st_len;
        //-- пропустить все пустые подстроки справа
    for( ; st_len > 1; st_len--){
        pc_src--;
        if(  *pc_src =='|' )  continue;
        break;
    }
        //-- закрыть строку нулем
    pc_src[1] = 0;
    return  st_len;
};
//----------------------------------------------------------
//-- удалить все  пустые и пробельные подстроки справа
//-- возвращает длину получившейся строки или отриц код ошибки
int cutRightWhite(char* pc ) {    
        //-- выполняем проверки
    if( !pc ) return - BAD_PTR;    
    int     st_len = getLen( pc);
    if( st_len <1 )  return 0;
    char    *pc_src = pc + st_len;
        //-- пропустить все пустые и пробельные подстроки справа
    for( ; st_len > 1; st_len--){
        pc_src--;
        if(  *pc_src =='|' )  continue;
        if(  *pc_src ==' ' )  continue;
        break;
    }
        //-- закрыть строку нулем
    pc_src[1] = 0;
    return  st_len;
};
//----------------------------------------------------------

