# Applied Distributed Computing - Project

<h3>Protocol</h3>

<P>
EVERY package contains a header and a JSON payload. The header was composed by 4 varibles: version, type, length and reserve.
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
     {"jsonkv": "1.0", "operate": "put", "key": "1“, "value": ”999“, "id": ”1“}
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
   <p>{"jsonkv": "1.0", "operate": "put", "key": "1“, "value": ”999“, "id": ”1“},</p>
   <p>{"jsonkv": "1.0", "operate": "get", "key": "2“, "value": ”“, "id": ”2“}</p>
   ]
   </p>
  <h2>batch operations response</h2>
   [
   <p>{"jsonkv": "1.0", "result": {"value":"0", "code":"0", "message": "put operation success"}, "id": "1"},</p>
   <p>{"jsonkv": "1.0", "result": {"value":"999", "code":"0", "message": "get operation success"}, "id": "2"}</p>
   ]
   </p>

<h3>UDP</h3>
<P>
Server will check every UDP package, if the package is larger or less than expected, the package will be dropped.

Client should check every response package. If the received package does not match the original length or there is no response under appropriate timeout, client have the responsibility to resend this request.
</P>