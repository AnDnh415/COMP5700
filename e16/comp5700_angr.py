import angr
import sys

def angr_solve():
    print("--- Starting Angr Analysis on 'comp5700' ---")
    
    # Load the binary
    # auto_load_libs=False speeds up analysis by ignoring system libraries
    project = angr.Project("./comp5700", auto_load_libs=False)

    # Create the starting state (Entry State)
    state = project.factory.entry_state()

    # Create the Simulation Manager (The engine that explores paths)
    sm = project.factory.simulation_manager(state)

    # Define success and failure conditions based on 'strings comp5700'
    def is_successful(state):
        output = state.posix.dumps(sys.stdout.fileno())
        return b"You have sucessfully used ANGR" in output

    def is_failure(state):
        output = state.posix.dumps(sys.stdout.fileno())
        return b"Access Denied" in output

    # Explore the binary
    sm.explore(find=is_successful, avoid=is_failure)

    # Extract and print the result
    if sm.found:
        found_state = sm.found[0]
        
        # Get the input (stdin) that created this state
        solution_input = found_state.posix.dumps(sys.stdin.fileno())
        
        print("\nSuccess! Vulnerability identified.")
        print(f"The password is: {solution_input.decode().strip()}")
    else:
        print("\nNo solution found!")

if __name__ == "__main__":
    angr_solve()