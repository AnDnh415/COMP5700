import logging
import functools
import sys

# %(asctime)s:%(message)s is the format for the log messages, according to the SAMPLE.log
logging.basicConfig(
    filename='app_forensics.log',
    level=logging.INFO,
    format='%(asctime)s:%(message)s'
)

# log configuration
logger = logging.getLogger()

def log_forensics(func):
    @functools.wraps(func)
    def wrapper(a, b):
        params_str = f"a:{a}, b:{b}"
        
        try:
            logger.info(f"{func.__name__} | The arguments that were provided -> {params_str}")
            
            return func(a, b)
            
        except Exception as e:
            # logger.exception() is the key. It automatically records the
            # full traceback, fulfilling the "Exception-related data" requirement.
            logger.exception(f"{func.__name__} | Exception occurred with arguments -> {params_str}")
            raise # Re-raise the error so the program knows it failed
            
    return wrapper

@log_forensics
def simpleDiv(a, b):
    return a/b 

@log_forensics
def mul(a, b):
    return a*b 

@log_forensics
def add(a, b):
    return a + b 

@log_forensics
def sub(a, b):
    return a - b 

if __name__ == "__main__":
    logger.info("This is for the demonstration of workshop 10.")
    logger.info("The calculator application has started...")
    
    print("Running functions to generate 'app_forensics.log'...")
    print("Running functions to generate 'app_forensics.log'...")
    
    # Run functions to generate log entries
    add(10, 5)
    sub(100, 50)
    mul(5, 5)
    simpleDiv(20, 4)
    
    # This try/except block is just to allow the script to
    # log the error and then finish gracefully instead of halting.
    try:
        simpleDiv(10, 0)
    except ZeroDivisionError:
        print("Successfully logged the expected division by zero error.")
        
    print("Log generation complete.")