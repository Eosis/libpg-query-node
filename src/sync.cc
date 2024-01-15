#include <napi.h>
#include <string>
#include <sstream>
#include "sync.h"  // NOLINT(build/include)
#include "helpers.h"  // NOLINT(build/include)
#include <list>

Napi::String ParseQuerySync(const Napi::CallbackInfo& info) {
  std::string query = info[0].As<Napi::String>();
  PgQueryParseResult result = pg_query_parse(query.c_str());

  return QueryParseResult(info.Env(), result);
}

Napi::String ParsePlPgSQLSync(const Napi::CallbackInfo& info) {
  std::string query = info[0].As<Napi::String>();
  PgQueryPlpgsqlParseResult result = pg_query_parse_plpgsql(query.c_str());

  return PlPgSQLParseResult(info.Env(), result);
}

Napi::String FingerprintSync(const Napi::CallbackInfo& info) {
  std::string query = info[0].As<Napi::String>();
  PgQueryFingerprintResult result = pg_query_fingerprint(query.c_str());

  return FingerprintResult(info.Env(), result);
}

Napi::String ScanQuerySync(const Napi::CallbackInfo& info) {
  std::string query = info[0].As<Napi::String>();
  PgQueryScanResult result = pg_query_scan(query.c_str());
  return QueryScanResult(info.Env(), query.c_str(), result);
}