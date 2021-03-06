// @@@ START COPYRIGHT @@@
//
// (C) Copyright 2006-2014 Hewlett-Packard Development Company, L.P.
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//
// @@@ END COPYRIGHT @@@

#ifndef XATMAPI_H_
#define XATMAPI_H_


int tm_xa_recover_send(int pv_rmid, int64 pv_count, int64 pv_flags,
                       int pv_index = 0, int pv_node=-1, bool pv_dead_tm=true);
int tm_xa_recover_waitReply(int *pp_rmid, XID *pp_xids, int64 *pp_count, 
                            bool *pp_end, int *pp_index, int pv_rm_wait_time, int *pp_int_error);
bool tm_xa_rmType_TSE(int pv_rmid);

bool tm_XARM_generic_library();

#endif //XATMAPI_H_
