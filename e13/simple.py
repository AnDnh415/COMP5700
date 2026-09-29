import random

def multiply(v1, v2):
    return v1 * v2

NAUGHTY_STRINGS = [
    # data types
    None,
    True,          
    False,          
    [1, 2, 3], 
    ('a', 'b'),     # Tuple
    {'key': 'val'}, # Dictionary
    {1, 2, 3},   
    b'bytes',       # Bytes
    complex(1, 2),  # Complex number

    # numeric types
    5,
    0,      
    -10,   
    3.14159,       
    "1",               
    "3.14",       
    "NaN",          
    float('inf'),   
    float('-inf'), 
    
    # empty strings
    "",         
    " ",            
    "\t",      
    "\n",       
    " \n \t ",    
    
    # unicode characters
    "😂",           
    "你好",        
    "Здравствуйте", 
    "삼성",         
    "Ångström",     # Special characters
    
    # XSS/ Injection strings
    "SELECT * FROM users;",         # SQL Injection
    "<script>alert('XSS')</script>", # Cross-Site Scripting (XSS)
    
    # strings
    "{}", 
    "[]",          
    "true",        
    "false",       
    "None"          
]

def simpleFuzzer():
    errors_found = [] 
    max_errors = 10
    
    print(f"Fuzzer starting... looking for {max_errors} unique errors.")
    
    while len(errors_found) < max_errors:
        v1 = random.choice(NAUGHTY_STRINGS)
        v2 = random.choice(NAUGHTY_STRINGS)
        
        try:
            multiply(v1, v2)
        except Exception as e:
            error_record = (
                f"--- CRASH #{len(errors_found) + 1} ---\n"
                f"INPUT (v1): {v1!r}\n"
                f"INPUT (v2): {v2!r}\n"
                f"ERROR TYPE: {type(e).__name__}\n"
                f"ERROR MSG:  {e}\n"
                f"---------------------\n\n"
            )
            if error_record not in errors_found:
                errors_found.append(error_record)
                print(f"Found unique error #{len(errors_found)}")

    # --- Write errors to file (with safety net) ---
    try:
        with open('errors.txt', 'w', encoding='utf-8') as f:
            for error in errors_found:
                f.write(error)
        
        print(f"\nFuzzing complete. Wrote {len(errors_found)} errors to errors.txt.")
    
    except Exception as e:
        print(f"Error writing to file: {e}")

if __name__=='__main__':
    simpleFuzzer()