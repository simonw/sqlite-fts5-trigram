/**********************************************************************/
#include "sqlite3ext.h"
SQLITE_EXTENSION_INIT1
#include <assert.h>
#include <string.h>
#include <fts5.h>

static fts5_api *fts5_api_from_db(sqlite3 *db){
  fts5_api *pRet = 0;
  sqlite3_stmt *pStmt = 0;

  if( SQLITE_OK==sqlite3_prepare(db, "SELECT fts5(?1)", -1, &pStmt, 0) ){
    sqlite3_bind_pointer(pStmt, 1, (void*)&pRet, "fts5_api_ptr", NULL);
    sqlite3_step(pStmt);
  }
  sqlite3_finalize(pStmt);
  return pRet;
}

static int dummy = 0;

static int ftsTriCreate(
  void *pCtx,
  const char **azArg,
  int nArg,
  Fts5Tokenizer **ppOut
){
  *ppOut = (Fts5Tokenizer*)&dummy;
  return SQLITE_OK;
}

static void ftsTriDelete(Fts5Tokenizer *p){
  assert( p==(Fts5Tokenizer*)&dummy );
}

static int ftsTriTokenize(
  Fts5Tokenizer *pUnused,
  void *pCtx,
  int flags,
  const char *pText, int nText,
  int (*xToken)(void*, int, const char*, int, int, int)
){
  int i;
  for(i=0; i<nText-2; i++){
    int rc = xToken(pCtx, 0, &pText[i], 3, i, i+3);
    if( rc ) return rc;
  }
  return SQLITE_OK;
}

static int ftsTriInstall(sqlite3 *db){
  fts5_api *pApi;
  fts5_tokenizer tok;
  tok.xCreate = ftsTriCreate;
  tok.xDelete = ftsTriDelete;
  tok.xTokenize = ftsTriTokenize;

  pApi = fts5_api_from_db(db);
  if( pApi==0 ) return 0;

  return pApi->xCreateTokenizer(pApi, "tri", 0, &tok, 0);
}

#ifdef _WIN32
__declspec(dllexport)
#endif
int sqlite3_ftstri_init(
  sqlite3 *db,
  char **pzErrMsg,
  const sqlite3_api_routines *pApi
){
  SQLITE_EXTENSION_INIT2(pApi);
  return ftsTriInstall(db);
}
