/*
 * Copyright 2019 SiFive, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You should have received a copy of LICENSE.Apache2 along with
 * this software. If not, you may obtain a copy at
 *
 *    https://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef PRIM_H
#define PRIM_H

#include "type.h"
#include "primfn.h"
#include <map>
#include <string>
#include <vector>
#include <memory>

struct Receiver;
struct Value;
struct String;
struct Integer;
struct Double;
struct RegExp;
struct Data;

/* Macros for handling inputs from wake */
#define RETURN(val) do {						\
  Receiver::receive(queue, std::move(completion), std::move(val));	\
  return;								\
} while (0)

std::unique_ptr<Receiver> require(const char *fn, WorkQueue &queue, std::unique_ptr<Receiver> completion, const std::shared_ptr<Binding> &binding, bool ok, const std::string &str);
#define REQUIRE(b, str) do {								\
  completion = require(__FUNCTION__, queue, std::move(completion), binding, b, str);	\
  if (!completion) return;								\
} while (0)

std::unique_ptr<Receiver> expect_args(const char *fn, WorkQueue &queue, std::unique_ptr<Receiver> completion, const std::shared_ptr<Binding> &binding, const std::vector<std::shared_ptr<Value> > &args, int expect);
#define EXPECT(num) do { 									\
  (void)data;											\
  completion = expect_args(__FUNCTION__, queue, std::move(completion), binding, args, num);	\
  if (!completion) return;									\
} while (0)

std::unique_ptr<Receiver> cast_string(WorkQueue &queue, std::unique_ptr<Receiver> completion, const std::shared_ptr<Binding> &binding, const std::shared_ptr<Value> &value, String **str);
#define STRING(arg, i) 									\
  String *arg;										\
  do {											\
    completion = cast_string(queue, std::move(completion), binding, args[i], &arg);	\
    if (!completion) return;								\
  } while(0)

std::unique_ptr<Receiver> cast_integer(WorkQueue &queue, std::unique_ptr<Receiver> completion, const std::shared_ptr<Binding> &binding, const std::shared_ptr<Value> &value, Integer **str);
#define INTEGER(arg, i) 								\
  Integer *arg;										\
  do {											\
    completion = cast_integer(queue, std::move(completion), binding, args[i], &arg);	\
    if (!completion) return;								\
  } while(0)

std::unique_ptr<Receiver> cast_double(WorkQueue &queue, std::unique_ptr<Receiver> completion, const std::shared_ptr<Binding> &binding, const std::shared_ptr<Value> &value, Double **str);
#define DOUBLE(arg, i) 									\
  Double *arg;										\
  do {											\
    completion = cast_double(queue, std::move(completion), binding, args[i], &arg);	\
    if (!completion) return;								\
  } while(0)

std::unique_ptr<Receiver> cast_regexp(WorkQueue &queue, std::unique_ptr<Receiver> completion, const std::shared_ptr<Binding> &binding, const std::shared_ptr<Value> &value, RegExp **reg);
#define REGEXP(arg, i)	 									\
  RegExp *arg;											\
  do {												\
    completion = cast_regexp(queue, std::move(completion), binding, args[i], &arg);		\
    if (!completion) return;									\
  } while(0)

std::unique_ptr<Receiver> cast_data(WorkQueue &queue, std::unique_ptr<Receiver> completion, const std::shared_ptr<Binding> &binding, const std::shared_ptr<Value> &value, Data **data);
#define DATA(arg, i) 									\
  Data *arg;										\
  do {											\
    completion = cast_data(queue, std::move(completion), binding, args[i], &arg);	\
    if (!completion) return;								\
  } while(0)

/* Useful expressions for primitives */
std::shared_ptr<Value> make_unit();
std::shared_ptr<Value> make_bool(bool x);
std::shared_ptr<Value> make_order(int x);
std::shared_ptr<Value> make_tuple2(std::shared_ptr<Value> &&first, std::shared_ptr<Value> &&second);
std::shared_ptr<Value> make_list(std::vector<std::shared_ptr<Value> > &&values);
std::shared_ptr<Value> make_result(bool ok, std::shared_ptr<Value> &&value);

#define PRIM_PURE	1	// has no side-effects (can be duplicated / removed)
#define PRIM_SHALLOW	2	// only wait for direct arguments (not children)

/* Register primitive functions */
struct PrimDesc {
  PrimFn   fn;
  PrimType type;
  int      flags;
  void    *data;

  PrimDesc(PrimFn fn_, PrimType type_, int flags_, void *data_ = 0)
   : fn(fn_), type(type_), flags(flags_), data(data_) { }
};

typedef std::map<std::string, PrimDesc> PrimMap;
struct JobTable;

void prim_register(PrimMap &pmap, const char *key, PrimFn fn, PrimType type, int flags, void *data = 0);
void prim_register_string(PrimMap &pmap, const char *version);
void prim_register_vector(PrimMap &pmap);
void prim_register_integer(PrimMap &pmap);
void prim_register_double(PrimMap &pmap);
void prim_register_exception(PrimMap &pmap);
void prim_register_regexp(PrimMap &pmap);
void prim_register_json(PrimMap &pmap);
void prim_register_job(JobTable *jobtable, PrimMap &pmap);
void prim_register_sources(std::vector<std::shared_ptr<String> > *sources, PrimMap &pmap);

#endif
