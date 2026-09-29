import subprocess
from allpairspy import AllPairs

# define the parameter options based on 'cppcheck -h'
parameters = [
    # --enable flags
    ["--enable=warnings", "--enable=unusedFunction", "--enable=performance", "--enable=portability"],
    # --std flags
    ["--std=c++11", "--std=c++14", "--std=c++17", "--std=c++20"],
    # --platform flags
    ["--platform=unix64", "--platform=win64", "--platform=arv8", "--platform=mips32"]
]

def run_pairwise_tests():
    log_file = "execution_log.txt"
    print("Generating pairwise combinations and executing tests...")
    with open(log_file, "w") as f:
        for i, (enable, std, platform) in enumerate(AllPairs(parameters)):
            # build command
            cmd = ["cppchecl", enable, std, platform, "HPC-Compiler-Fuzzers"] # folder name after unzip
            f.write(f"--- Test Case #{i+1}: {' '.join(cmd)} ---\n")
            try:
                result = subprocess.run(cmd, capture_output=True, text=True)
                f.write(f"Exit Code: {result.returncode}\n")
                # limit output to first 20 lines
                f.write("\n".join(result.stderr.splitlines()[:20]) + "\n")
            except Exception as e:
                f.write(f"Error executing command: {e}\n")
    print(f"Testing completed.")

if __name__ == "__main__":
    run_pairwise_tests()