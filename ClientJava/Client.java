
public class Client {
	private Integer code = null;  
    private String message = null;  
    private String value = null;  
      
    public Client(){	
    }  
    public Client(Integer code, String message) {  
        super();  
        this.code = code;  
        this.message = message;   
    }  
    public Client(Integer code, String message, String value) {  
        super();  
        this.code = code;  
        this.message = message;  
        this.value = value;  
    }  
    public Integer getCode() {  
        return code;  
    }  
    public void setCode(Integer code) {  
        this.code = code;  
    }  
    public String getMessage() {  
        return message;  
    }  
    public void setMessage(String message) {  
        this.message = message;  
    }  
    public String getValue() {  
        return value;  
    }  
    public void setValue(String value) {  
        this.value = value;  
    }  
    @Override  
    public String toString() {  
    	if (value != null)
    		return "[code=" + code + ", message=" + message + ", value=" + value + "]";  
    	else
    		return "[code=" + code + ", message=" + message + "]";
    }  
      
}
