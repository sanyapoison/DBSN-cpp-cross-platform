#ifndef DBSN_H
#define DBSN_H

#define     REC_DELETED      1
#define     OUT_OF_RANGE     2
#define     DBN_IS_EMPTY     4
#define     REC_EMPTY        8
#define     BAD_SUBSTNUMBER  16

#define     FATAL_ERROR      256
#define     BAD_PTR          256
#define     BAD_NUMBER       512
#define     DBS_NOT_EXIST    1024
#define     DBN_NOT_EXIST    2048
#define     BUFFER_SMALL     4096
#define     BAD_TABLE_NAME   8192
#define     BAD_TABLE_HANDLE 16384
#define     DBR_NOT_EXIST    32768
#define     NOT_A_BRACKET    65536

#define     ALL_FRAGM 0
#define     ACTIVE_FRAGM 1
#define     ALL_BACK 2
#define     ACTIVE_BACK 3

#define     BEG_BRACKET      0x80000000
#define     END_BRACKET      0x00000000

#include <sys/types.h> //chsise -> ftruncate
#include <unistd.h>  // close
#include <string.h>
#include <ctype.h>   // toupper
#include <stdio.h>

//---------------------------------------------------------------------------

//-- создать пустой массив целых чисел
extern "C" int        
	getLongArray( int count );

//-- сравнить 2 строки с учетом подстрок и игнорирование регистра
extern "C" long         
	compSpec( char* pc, char* pc1);

//-- сравнить нулевые подстроки 2х строк с игнорированием регистра
extern "C" long     
	compSpec0( char* pc, char* pc1);

//-- сосчитать длину строки
extern "C" int     
	getLen( char* pc);

//-- сосчитать число подстрок в строке
extern "C" int     
	substCount( char* pc);

//-- получить смещение подстроки с заданным номером
extern "C" int     
	getSubstOffset(char* pc, int nom_sub);

//-- проверить подстроку на вхождение шаблона
extern "C" int     
	checkSubst(  char* pc, char* templ, int nom_sub );

//-- вытащить подстроку с заданным номером из строки
extern "C" int     
	getSubst(char* pc, char* pcbuf, int buf_size, int nom_sub);

//-- вытащить целое число из подстроки с заданным номером
extern "C" int     
	getSubint(char* pc, int& ires, int nom_sub);

//-- сформировать строку в буфере из целого числа
extern "C" int     
	formStint( char* pcbuf, int buf_size, int ival );

//== добавить подстроку к строке сзади
extern "C" int     
    addSubst(char* pc, char* pcres, int res_size, char* pcsubst);

//== добавить числовую подстроку к строке сзади
extern "C" int     
    addSubint(char* pc, char* pcres, int res_size, int ival);

//== вставить подстроку в нужное место строки
extern "C" int     
    insSubst(char* pc, char* pcres, int res_size, int nom_sub, char* pcsubst);

//== вставить числовую подстроку в нужное место строки
extern "C" int     
    insSubint(char* pc, char* pcres, int res_size, int nom_sub,  int ival);

//== получить длину подстроки
extern "C" int     
    substLen(char* pc, int nom_sub );

//-- заменить подстроку на новое значение в нужном месте строки
extern "C" int     
	setSubst(char* pc, char* pcres, int res_size, int nom_sub, char* pcsubst);

//-- заменить числовую подстроку на новое значение в нужном месте строки
extern "C" int     
	setSubint(char* pc, char* pcres, int res_size, int nom_sub,  int ival );

//-- очистить подстроку (заменить на пустое значение )
extern "C" int     
    clearSubst(char* pc, int nom_sub );

//-- полностью удалить подстроку
extern "C" int     
    cutSubst(char* pc, int nom_sub );

//-- полностью удалить группу подстрок
extern "C" int     
    cutSubstGroup(char* pc, int nom_sub, int cut_count );

//-- удалить все подстроки перед заданой
extern "C" int     
    cutLeft(char* pc, int nom_sub );

//-- удалить все подстроки начиная с заданой
extern "C" int     
    cutRight(char* pc, int nom_sub );

//-- удалить все пустые подстроки справа
extern "C" int     
    cutRightEmpty(char* pc );

//-- удалить все пустые  и пробельные подстроки справа
extern "C" int     
    cutRightWhile(char* pc );

//-- объединить подстроку с предидущей
extern "C" int     
    uniteSubst(char* pc, int nom_sub );

//-- проверить условие для 2х подстрок
extern "C" int     
    compSubst(  char* pc1, char* pc2, int subst_nom, char* condition );

//==  получить начальную строку текста( заголовок)
extern "C" int       getTextHead(char* pc, char* pcbuf, int bufsize);

//==  получить остаток текста без начальной строки( заголовка )
extern "C" int       getTextBody(char* pc, char* pcbuf, int bufsize);

//-- запись целого числа в бинарный файл
extern "C" int         
	fwriteInt( int fh, int val);

//-- чтение целого числа из бинарного файла
extern "C" int         
	freadInt( int fh );

//== сосчитать число строк в тексте =
extern "C" int         
	countTextSt(char* pc);

//== получить строку с заданным номером из текста
extern "C" int       
	getTextSt(char* pc, char* pcbuf, int bufsize,  int st_nom);

//== Считать Смещение текущего фрагмента базы DBSN 
extern "C" int         
	getFragmOffset( int  dbhadr,  int &rec_offset );

//== Сосчитать Длину  текущего фрагмента базы DBSN
extern "C" int     
	getFragmSize( int  dbhadr, int &rec_size );

//== Сосчитать максимальную длину фрагмента базы DBSN
extern "C" int         
	getMaxFragmSize( int  dbhadr );

//== проверить активна ли запись
extern "C" int         
	isActive( int dbhadr );

//==  получить  признак ошибки DBSN
extern "C" int         
	getErrCod( int dbhadr );

//== очистить признак ошибки
extern "C" int         
	clearError( int dbhadr );

//-- новая функция ----
extern "C" char*       
	trim( char* pc, char* pcres);

//== создать пустую базу фрагментов по имени файла
extern "C" int          
	createDBSN( char*  fil_name );

//== проверить существование файлов DBSN
extern "C" int          
	existDBSN( char*  fil_name );

//== получить все содержимое DBS файла
extern "C" int       
	getAllDBS( char* fil_name, char* &text );

//== открыть базу фрагментов DBSN
extern "C" int       
	openDBSNA( char* fil_name );

//== открыть базу фрагментов DBSN и встать на первый активный фрагм
extern "C" int    
	openDBSN( char* fil_name );

//== Закрыть базу фрагментов DBSN
extern "C" int       
	closeDBSN( int &dbhadr );

//==  Сосчитать число всех записей в Базе DBSN
extern "C" int     
	countFragmA( int dbhadr );

//==  Сосчитать число записей (активных ) в Базе DBSN
extern "C" int     
	countFragm( int dbhadr );

//== Удалить все фрагменты базы DBSN
extern "C" int     
	delAllFragm(  int dbhadr );

//== Полностью уничтожить  базу DBSN
extern "C" int       
	eraseDBSN( char*  fil_name);

//==  Переименовать базу DBSN в новое имя
extern "C" int        
	renameDBSN( char*  old_fil, char*  new_fil );

//== Упаковать базу DBSN
extern "C" int     
	packDBSN( char*  fil_name );

//== сделать выборку из базы DBSN  по активным ссылкам
extern "C" int     
    selectDBSN(  int dbhadr, int fhr, char* new_tabl );

//== проверить на конец файла базу DBSN
extern "C" int        
	isEndDBSN( int dbhadr );

//==  получить номер текущего фрагмента
extern "C" int        
	getNom(int dbhadr );

//==  установить номер текущего фрагмента
extern "C" int      	 
	setNom(int dbhadr, int new_nom );

//==  увеличить номер текущего фрагмента  
extern "C" int        
	incNom(int dbhadr, int  count );

//==  уменьшить номер текущего фрагмента
extern "C" int        
	decNom(int dbhadr, int  count );

//==  установить первый номер фрагмента
extern "C" int        
	setFirst(int dbhadr );

//==  установить последний номер фрагмента
extern "C" int        
	setLast(int dbhadr );

//== Считать текущий фрагмент базы DBSN  
extern "C" int     
	getFragm(  int dbhadr, char* fragm,  int bufsize );

//== Прочитать первый фрагмент базы DBSN
extern "C" int     
	getFirstFragm(  int dbhadr, char* fragm,  int bufsize );

//== Прочитать следующий фрагмент базы DBSN
extern "C" int     
	getNextFragm(  int dbhadr, char* fragm,  int bufsize );

//== Прочитать предыдующий фрагмент базы DBSN
extern "C" int     
	getPrevFragm(  int dbhadr, char* fragm,  int bufsize );

//== Прочитать первый фрагмент базы DBSN
extern "C" int     
	getLastFragm(  int dbhadr, char* fragm,  int bufsize );

//== Прочитать заданный фрагмент базы DBSN
extern "C" int     
	readFragm( int* dbh, char* fragm, int bufsize, int fragm_nom );

//==  Добавить запись в базу фрагментов DBSN
extern "C" int     
    addFragm( int dbhadr, char* fragm );

//==  Записать текст в текущую запись базы DBSN
extern "C" int     
	setFragm(  int dbhadr, char*  fragm );

//==  Записать текст в заданную запись базы DBSN
extern "C" int     
	writeFragm(int dbhadr, char* fragm, int fragm_nom );

//== Удалить фрагмент базы DBSN с заданным номером
extern "C" int     
	delFragm(  int dbhadr );

//== Снять признак удаления с фрагмента базы DBSN
extern "C" int        
	undelFragm(  int dbhadr );

//== Сосчитать число блоков фрагментов базы DBSN
extern "C" int        
	countNblocks( int dbhadr, int block_size, int flag_sel );

//== Извлечь заданный блок номеров фрагментов базы DBSN
extern "C" int        
	getNblock( int dbhadr, int* pblock, int block_nom,  int block_size, int flag_sel  );

//-- сосчитать число подстрок в бинарной строке
extern "C" int      
	binsubstCount( char* pcFragm, int fragmLen);

//-- получить смещение бинарной подстроки
extern "C" int     
	binsubstOffset(char* pcFragm, int fragmLen, long nom_sub);

//-- сосчитать длину подстроки в байтах по ее номеру
extern "C" int     
	binsubstLen( char* pcFragm, int fragmLen, long nom_sub );

//-- сосчитать размер подстроки в словах по ее номеру
extern "C" int     
	binsubstSize( char* pcFragm, int fragmLen, long nom_sub );

//-- вытащить бинарную подстроку с заданным номером из строки в буфер
extern "C" int     
	getBinsubst(char* pcFragm, int fragmLen, char* pcBuf, int bufLen, long nom_sub);

//== добпвить к фрагменту бинарную подстроку
extern "C" int     
	addBinsubst( char* pcFragm, int fragmLen, char* pcBuf, int bufLen,  char* pcBinsubst );

//== вырезать бинарную подстроку из фрагмента
extern "C" int     
	cutBinsubst( char* pcFragm, int fragmLen, long nom_sub );

//== очистить бинарную подстроку  в фрагменте
extern "C" int     
    clearBinsubstIn( char* pcFragm, int fragmLen, long nom_sub );

//== вставить бинарную подстроку перед заданной во фрагмент
extern "C" int     
	insBinsubst( char* pcFragm, int fragmLen, char* pcBuf, int bufLen, long nom_sub, char* pcBinsubst );

extern "C" int     
	setBinsubst( char* pcFragm, int fragmLen, char* pcBuf, int bufLen, long nom_sub, char* pcBinSubst);

//==  Добавить Бин запись в базу фрагментов DBSN
extern "C" int          
	addBinFragm( int dbhadr, char* fragm, int len_fragm );

//==  Записать Бин фрагмент в текущую запись базы DBSN
extern "C" int        
	setBinFragm(  int dbhadr, char*  fragm, int len_fragm );

//==  перейти к первому активному фрагменту
extern "C" int     
    gotoFirst( int dbhadr );

//==  перейти вперед через несколько активных фрагментов
extern "C" int     
    goForward( int dbhadr, int count );

//==  перейти назад через несколько активных фрагментов
extern "C" int     
    goBack( int dbhadr, int count );

//==  перейти к последнему активному фрагменту
extern "C" int     
    gotoLast( int dbhadr );

//==  перейти вперед к следующему активному фрагменту
extern "C" int     
    gotoNext( int dbhadr );

//==  перейти назад к предидущему активному фрагменту
extern "C" int     
    gotoPrev( int dbhadr );

extern "C" int*     
	writeBinRec( int dbhadr, int* plbuf);

extern "C" int*      
	readBinRec( int dbhadr, int* plbuf);

extern "C" int      
	xchgFragm( int  dbhadr, int  dest_nom );

extern "C" int      
	flushDBSN( int  dbhadr );

//== Полностью вырезать данные фрагмента  из dbn
extern "C" int     
	cutFragm(  int dbhadr );

//==  Вставить запись в базу фрагментов DBSN в текущую позицию
extern "C" int     
	insFragm( int dbhadr, char* fragm);

//== Переместить текущий фрагмента в заданное место dbn
extern "C" int     
	moveFragm(  int dbhadr, int  dest_nom );

//== Поменять местами текущий и заданый фрагменты
extern "C" int     
	xchgFragm( int  dbhadr, int  dest_nom );

//== скопировать базу фрагментов под другим именем
extern "C" int     
	copyDBSN( char*  fil_name, char*  copy_filname, int flag_pack  );

//== дозаписать в буфер описание ошибки с комментарием
extern "C" int     
	getErrMes( int dbhadr, char* err_buf, int buf_len, char* comment);

//== работа со ссылками

//== создать ссылочный файл на таблицу фрагментов по имени файла
extern "C" int    
	createDBRF( char*  fil_name);

//== открыть ссылочный файл на таблицу фрагментов DBSN
extern "C" int    
	openDBRF( char* filref_name );

//== сформировать ссылочный файл по DBSN и условию
extern "C" int    
	formDBRF(  int dbhadr, int fhr, int subst_nom, char* condition, char* value );

//== закрыть ссылочный файл на таблицу фрагментов DBSN
extern "C" int   
	closeDBRF( int fhr );

//== зскопировать ссылочный файл в новый с возможной упаковкой
extern "C" int   
	copyDBRF( char*  fil_ref, char*  copy_ref, int flag_pack );

//== упаковать файл ссылок
extern "C" int    
    packDBRF(  int fhr );

//== подсчитать количество записей в файле ссылое
extern "C" int    
	countRef( int fhr );

//== получить номер фрагмента по ссылке на него
extern "C" int     
	getRef(  int fhr, int ref_nom );

//== получить фрагмент по ссылке на него
extern "C" int     
	getFragmByRef(  int dbhadr, int fhr, int ref_nom,  char* fragm, int bufsize );

//== добавить в ссылочный файл номер фрагмента
extern "C" int     
	addRef(  int fhr, int fragm_nom );

//== заменить в ссылочном файле номер фрагмента в нужной позиции
extern "C" int     
	setRef(  int fhr, int ref_nom, int fragm_nom );

//== удалить в ссылочном файле номер фрагмента в нужной позиции
extern "C" int     
	delRef(  int fhr, int ref_nom );

//== Снять удаление в ссылочном файле
extern "C" int     
	undelRef(  int fhr, int ref_nom );

//== Поменять местами ссылки на фрагменты
extern "C" int       
	xchgRef( int  fhr, int  ref_nom1,  int ref_nom2 );

//==  вырезать физически в ссылочном файле номер фрагмента в нужной позиции
extern "C" int     
	cutRef(  int fhr, int ref_nom );

//== вставить в ссылочном файле номер фрагмента в нужной позиции
extern "C" int    
	insRef(  int fhr, int ref_nom, int fragm_nom );

//== вставить в ссылочном файле номер фрагмента после нужной позиции
extern "C" int    
	insRefAfter(  int fhr, int ref_nom, int fragm_nom );
	
//== переместить ссылку в новуй позицию со сдвигом
extern "C" int    
	moveRef(  int fhr, int ref_nom1, int ref_nom2 );

//== вдавить блок ссылок на следующий уровень
extern "C" int    
	pushRblock(  int fhr, int ref_nom, int block_len );

//== найти начало блока( открывающую скобку )
extern "C" int    
	findRblock(  int fhr, int ref_nom );

//== найти соответствующую закрывающую скобку
extern "C" int    
	endRblock(  int fhr, int ref_nom );

//== найти соответствующую открывающую скобку (незад)
extern "C" int    
	begRblock(  int fhr, int ref_nom );

//== вынуть блок ссылок на предидущий уровень
extern "C" int    
	popRblock(  int fhr, int ref_nom );

//== вырезать блок из файла ссылок
extern "C" int    
	cutRblock(  int fhr, int ref_nom, int block_len );

//== переместить блок ссылок в новую позицию файла
extern "C" int    
	moveRblock(  int fhr, int new_nom, int ref_nom, int block_len );

//== добавить блок ссылок из другого файла
extern "C" int    
	addRblock(  int fhr, int fhr2, int ref_nom2, int block_len );

//== вставить блок ссылок из другого файла
extern "C" int    
	insRblock(  int fhr, int ref_nom,int fhr2, int ref_nom2, int block_len );

//== очистить фильтр на ссылочном файле
extern "C" int    
    clearFiltr(  int fhr  );

//== добавить фильтр на ссылочный файл
extern "C" int    
    addFiltr(  int dbhadr, int fhr, int subst_nom, char* condition, char* value );

//== отсортировать ссылочный файл по условию
extern "C" int    
    sortDBRF(  int dbhadr, int fhr, int subst_nom, char* condition );


//== установить новое число в заданное место нужной бинарной подстроки
extern "C" int    
    setBinnum(char* pcFragm, int fragmLen, long nom_sub, long nom_num, long src );

//== получить число с заданным номером из нужной бинарной подстроки
extern "C" int    
    getBinnum(char* pcFragm, int fragmLen, long nom_sub, long nom_num, long &dst );

//== удалить число с заданным номером из нужной бинарной подстроки
extern "C" int    
    cutBinnum(char* pcFragm, int fragmLen, long nom_sub, long nom_num );

//== добавить новое число в конец нужной бинарной подстроки
extern "C" int    
    addBinnum(char* pcFragm, int fragmLen, char* pcBuf, int lenBuf, long nom_sub, long src );

//== вставить новое число в заданное место нужной бинарной подстроки
extern "C" int    
    insBinnum(char* pcFragm, int fragmLen, char* pcBuf, int lenBuf, long nom_sub, long nom_num, long src );

//== поменять местами 2 числа в нужной бинарной подстроке
extern "C" int    
    xchgBinnum(char* pcFragm, int fragmLen, long nom_sub, long nom_num1, long nom_num2 );

//== разбить DBS файл на фрагменты по ключевому слову
extern "C" int    
	formDBSN( char* txtFile, char* keyWord, int add_key);

#endif