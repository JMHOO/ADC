# Applied Distributed Computing - Project

<h3>Protocol</h3>

<P>
EVERY package contains a header and a JSON payload. The header was composed by 4 variables: version, type, length and reserve. 
The header.Length = len(Header itself) + len(JSON payload).
</P>

<table>
    <tr>
        <td>4 bytes</td>
        <td>4 bytes</td>
        <td>4 bytes</td>
        <td>4 bytes</td>
        <td></td>
    </tr>
    <tr>
        <td>version</td>
        <td>type</td>
        <td>length</td>
        <td>reserve</td>
        <td width=60%>JSON payload</td>
    </tr>
</table>
<P>
For example: 
<blockquote>
{"jsonkv":"1.0","operate":"put","key":"tomato","value":"999","id":"1"}
</blockquote>
</P>
<P>
The length of JSON itself is 65, and the Header's size is an constant which always be 16. In this case, the bytes stream of this package looks like this:
<table>
    <tr>
        <td>Version</td>
        <td>Type</td>
        <td>Length</td>
        <td>Reserve</td>
        <td>JSON payload</td>
    </tr>
    <tr>
        <td>0</td>
        <td>0</td>
        <td>81</td>
        <td>0</td>
        <td>{"jsonkv":"1.0","operate":"put","key":"1","value":"999","id":"1"}</td>
    </tr>
</table>
</P>

<h3>JSON format request</h3>
  <p> <-- {"jsonkv": version, "operate": operations, "key": key, "value": value, "id": operation id}</p>
  <p>
      <table>
        <tr><td>version</td><td>1.0</td></tr>
        <tr><td>operate</td><td>put / get / delete</td></tr>
        <tr><td>key</td><td>string value</td></tr>
        <tr><td>value</td><td>string value</td></tr>
        <tr><td>id</td><td>operation id -- generated by client</td></tr>
      </table>
  </p> 
  <blockquote>
    Example:
     {"jsonkv":"1.0","operate":"put","key":"1","value":"999","id":"1"}
  </blockquote>
<h3>JSON format response</h3>
  <p> --> {"jsonkv": version, "result": {"value": value, "code": code, "message": message}, "id": operation id}</p>
  <p>
      <table>
        <tr><td>version</td><td>1.0</td></tr>
        <tr><td>result</td><td> </td></tr>
        <tr><td>result - value</td><td>only 'get' operation will return value<br>otherwise value = 0 or empty</td></tr>
        <tr><td>result - code</td><td> success : code == 0<br>errors: code > 0</td></tr>
        <tr><td>result - message</td><td>success : message is empty<br>errors: server will explain</td></tr>
        <tr><td>id</td><td>operation id -- generated by client</td></tr>
      </table>
  </p>
 <blockquote>
    Example:
     {"jsonkv": "1.0", "result": {"value": "0", "code": "1000", "message": "key does not exist.“, "id": ”1“}
  </blockquote>

  <h2>batch operations</h2>
   [
   <p>{"jsonkv":"1.0","operate":"put","key":"1","value":"999","id":"1"},</p>
   <p>{"jsonkv":"1.0","operate":"get","key":"2“,"value":"","id":"2"}</p>
   ]
   </p>
  <h2>batch operations response</h2>
   [
   <p>{"jsonkv": "1.0", "result": {"value":"0", "code":"0", "message": "put operation success"}, "id": "1"},</p>
   <p>{"jsonkv": "1.0", "result": {"value":"999", "code":"0", "message": "get operation success"}, "id": "2"}</p>
   ]
   </p>

<h3>Discovery JSON format request</h3>
  <p> <-- {"jsonagent": version, "operate": operations, "address": server address, "port": TCP server port}</p>
  <p>
      <table>
        <tr><td colspan="2"><B>Register</B></td></tr>
        <tr><td>version</td><td>1.0</td></tr>
        <tr><td>operate</td><td>register</td></tr>
        <tr><td>address</td><td>server address( internet ip or domain name )</td></tr>
        <tr><td>tcpport</td><td>TCP server port</td></tr>
        <tr><td>rpcport</td><td>RPC server port</td></tr>
        <tr><td colspan="2"><B>Unregister</B></td></tr>
        <tr><td>version</td><td>1.0</td></tr>
        <tr><td>operate</td><td>unregister</td></tr>
        <tr><td colspan="2"><B>GetServerList</B></td></tr>
        <tr><td>version</td><td>1.0</td></tr>
        <tr><td>operate</td><td>getserverlist</td></tr>
        <tr><td>protocol</td><td>tcp or rpc</td></tr>
      </table>
  </p> 
  <blockquote>
    Example:<BR/>
     {"jsonagent":"1.0","operate":"register","address":"uw.umx.io","tcpport":"15001","rpcport":"15003"}<BR/>
     {"jsonagent":"1.0","operate":"unregister"}<BR/>
     {"jsonagent":"1.0","operate":"getserverlist","protocol":"tcp"}<BR/>
  </blockquote>
  
<h3>Discovery JSON format response</h3>
  <p> --> {"jsonagent": version, "result": {"value": [{"address":"uw.umx.io","port",15003},], "code": code, "message": message}}</p>
  <p>
      <table>
        <tr><td>version</td><td>1.0</td></tr>
        <tr><td>result</td><td> </td></tr>
        <tr><td>result - value</td><td>json array contains server list<BR>if request is register, value=global server id</td></tr>
        <tr><td>result - code</td><td> success : code == 0<br>errors: code > 0</td></tr>
        <tr><td>result - message</td><td>success : message is empty<br>errors: server will explain</td></tr>
      </table>
  </p>
 <blockquote>
    Example:<BR/>
    {"jsonagent":"1.0","result":{"value":"2","code":"0","message":"register success"}}<BR/>
    {"jsonagent":"1.0","result":{"code":"0","message":"unregister success"}}<BR/>
    {"jsonagent":"1.0","result":{"value":[{"address":"uw.umx.io","port":15003}], "code":"0","message":"get tcp/rpc server list success"}}
  </blockquote>
  
<h3>UDP</h3>
<P>
Server will check every UDP package, if the package is larger or less than expected, the package will be dropped.

Client should check every response package. If the received package does not match the original length or there is no response under appropriate timeout, client have the responsibility to resend this request.
</P>
