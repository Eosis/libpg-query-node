#include "helpers.h"  // NOLINT(build/include)
#include "pg_query.h"
#include "helpers.h"
#include <napi.h>
#include <string>
#include "protobuf/pg_query.pb-c.h"
#include <sstream>
#include <vector>

Napi::Error CreateError(Napi::Env env, const PgQueryError& err)
{
    auto error = Napi::Error::New(env, err.message);
    error.Set("fileName", err.filename);
    error.Set("functionName", err.funcname);
    error.Set("lineNumber", Napi::Value::From(env, err.lineno));
    error.Set("cursorPosition", Napi::Value::From(env, err.cursorpos));
    error.Set("context", err.context ? Napi::Value::From(env, err.context) : env.Null());
    return error;
}

Napi::String QueryParseResult(Napi::Env env, const PgQueryParseResult& result)
{
    if (result.error) {
        auto throwVal = CreateError(env, *result.error);
        pg_query_free_parse_result(result);
        throw throwVal;
    }

    auto returnVal = Napi::String::New(env, result.parse_tree);
    pg_query_free_parse_result(result);
    return returnVal;
}

Napi::String PlPgSQLParseResult(Napi::Env env, const PgQueryPlpgsqlParseResult& result)
{
    if (result.error) {
        auto throwVal = CreateError(env, *result.error);
        pg_query_free_plpgsql_parse_result(result);
        throw throwVal;
    }

    auto returnVal = Napi::String::New(env, result.plpgsql_funcs);
    pg_query_free_plpgsql_parse_result(result);
    return returnVal;
}


Napi::String FingerprintResult(Napi::Env env, const PgQueryFingerprintResult & result)
{
  if (result.error) {
    auto throwVal = CreateError(env, *result.error);
    pg_query_free_fingerprint_result(result);
    throw throwVal;
  }

  auto returnVal = Napi::String::New(env, result.fingerprint_str);
  pg_query_free_fingerprint_result(result);
  return returnVal;
}

std::string ConvertQueryScanFromProtobuf(const char * input, const PgQueryScanResult& result) {
  PgQuery__ScanResult *scan_result = pg_query__scan_result__unpack(NULL, result.pbuf.len, (const unsigned char *) result.pbuf.data);
  PgQuery__ScanToken *scan_token;
  const ProtobufCEnumValue *token_kind;
  const ProtobufCEnumValue *keyword_kind;


  std::vector<std::string> tokens = {};
  for (size_t j = 0; j < scan_result->n_tokens; j++) {
    std::stringstream token_description;
    scan_token = scan_result->tokens[j];
    token_kind = protobuf_c_enum_descriptor_get_value(&pg_query__token__descriptor, scan_token->token);
    keyword_kind = protobuf_c_enum_descriptor_get_value(&pg_query__keyword_kind__descriptor, scan_token->keyword_kind);
    token_description << "["
     << "\"" << std::string(&input[scan_token->start], scan_token->end - scan_token->start)  << "\""
     << ", "
     << scan_token->start
     << ", "
     << scan_token->end
     << ", "
     << "\"" << token_kind->name << "\""
     << ", "
     << "\"" << keyword_kind->name << "\""
     << "]";
    
    tokens.push_back(token_description.str());
    // printf("  \"%.*s\" = [ %d, %d, %s, %s ]\n", scan_token->end - scan_token->start, &(input[scan_token->start]), scan_token->start, scan_token->end, token_kind->name, keyword_kind->name);
  }

    pg_query__scan_result__free_unpacked(scan_result, NULL);

  
  std::stringstream output;
  output << "[";
  for (size_t j = 0; j < tokens.size(); j++) {
    if (j != 0) {
      output << ", " ;
    }
      output << tokens[j];
  }
  output << "]";
  return output.str();
}

Napi::String QueryScanResult(Napi::Env env, const char * input, const PgQueryScanResult& result)
{
  if (result.error) {
    auto throwVal = CreateError(env, *result.error);
    pg_query_free_scan_result(result);
    throw throwVal;
  }

  auto returnVal = Napi::String::New(env, ConvertQueryScanFromProtobuf(input, result));
  pg_query_free_scan_result(result);

  return returnVal;
}