struct ResultMsg {
	1: i32 code = 0,
	2: string message,
	3: string value
}

service KVService {
	ResultMsg Put(1:string key, 2:string value),
	ResultMsg Get(1:string key),
	ResultMsg Delete(1:string key)
}	
