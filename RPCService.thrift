struct ResultMsg {
	1: i32 code = 0,
	2: string message,
	3: string value
}

service KVService {
	ResultMsg Put(1:string key, 2:string value),
	ResultMsg Get(1:string key),
	ResultMsg Delete(1:string key),
	ResultMsg SrvPut(1:i32 coordid, 2:i32 clientid, 3:string key, 4:string value),
	ResultMsg SrvDelete(1:i32 coorid, 2:i32 clientid, 3:string key),
	ResultMsg SrvGo(1:i32 coorid, 2:i32 clientid)
}	
