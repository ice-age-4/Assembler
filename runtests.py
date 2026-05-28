import os
import sys
import subprocess
import glob

UPDATE_MODE = "--update" in sys.argv or "-u" in sys.argv

def compile_project():
    print("Building project...")
    make_cmd = "mingw32-make" if os.name == "nt" else "make"
    subprocess.run([make_cmd, "clean"], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    subprocess.run([make_cmd], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)

def clean_old_actuals():
    for f in glob.glob("examples/invalid/*.actual") + glob.glob("examples/valid/*.actual") + glob.glob("examples/*.actual"):
        try: os.remove(f)
        except OSError: pass

def run_and_verify(test_name, input_files, expected_path, actual_path):
    executable = "./assembler" if os.name != "nt" else "assembler.exe"
    
    cleaned_args = [os.path.splitext(f)[0] if f.endswith(".as") else f for f in input_files]
    
    result = subprocess.run([executable] + cleaned_args, capture_output=True, text=True)
    full_output = result.stdout + result.stderr

    if UPDATE_MODE:
        with open(expected_path, "w", encoding="utf-8") as f:
            f.write(full_output)
        print(f"[*] GENERATED/UPDATED: {expected_path}")
        return True
    else:
        if not os.path.exists(expected_path):
            print(f"[-] Skipped [{test_name}]: Missing .expected file")
            return None

        with open(expected_path, "r", encoding="utf-8") as f:
            expected_content = f.read()

        if expected_content.strip() == full_output.strip():
            print(f"[+] PASSED: {test_name}")
            return True
        else:
            print(f"[X] FAILED: {test_name} (Output mismatch!)")
            with open(actual_path, "w", encoding="utf-8") as f:
                f.write(full_output)
            return False

def main():
    compile_project()
    clean_old_actuals()
    
    total_tests = 0
    failed_tests = 0

    print("\n=== Running Valid Input Tests ===")
    for as_file in glob.glob("examples/valid/*.as"):
        total_tests += 1
        base_name = os.path.splitext(os.path.basename(as_file))[0]
        expected = f"examples/valid/{base_name}.expected"
        actual = f"examples/valid/{base_name}.actual"
        
        res = run_and_verify(base_name, [as_file], expected, actual)
        if res is False: failed_tests += 1

    print("\n=== Running Invalid Input Tests ===")
    for as_file in glob.glob("examples/invalid/*.as"):
        total_tests += 1
        base_name = os.path.splitext(os.path.basename(as_file))[0]
        expected = f"examples/invalid/{base_name}.expected"
        actual = f"examples/invalid/{base_name}.actual"
        
        res = run_and_verify(base_name, [as_file], expected, actual)
        if res is False: failed_tests += 1

    print("\n=== Running Missing File Error Test ===")
    total_tests += 1
    fake_file = "examples/invalid/this_file_does_not_exist"
    expected_missing = "examples/invalid/missing_file_error.expected"
    actual_missing   = "examples/invalid/missing_file_error.actual"
    
    res = run_and_verify("Missing File Validation", [fake_file], expected_missing, actual_missing)
    if res is False: failed_tests += 1

    print("\n=== Running Multi-File Integration Test ===")
    total_tests += 1
    all_as_files = glob.glob("examples/valid/*.as") + glob.glob("examples/invalid/*.as")
    
    res = run_and_verify("Multi-File Integration", all_as_files, "examples/multi_run.expected", "examples/multi_run.actual")
    if res is False: failed_tests += 1

    print("\n=== Test Summary ===")
    if UPDATE_MODE:
        print("All expected snapshots have been updated successfully. 👍")
    else:
        print(f"Total tests evaluated: {total_tests}")
        if failed_tests == 0:
            print("Result: ALL TESTS PASSED! 🎉")
        else:
            print(f"Result: {failed_tests} tests failed. Check .actual files for diffs. ❌")

if __name__ == "__main__":
    main()