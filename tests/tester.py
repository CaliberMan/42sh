import subprocess
from argparse import ArgumentParser
from difflib import unified_diff
from os import listdir
from os.path import isfile, join
from pathlib import Path
from dataclasses import dataclass

import termcolor

OK_TAG = f"[{termcolor.colored('ok', 'green')}]"
KO_TAG = f"[{termcolor.colored('KO', 'red')}]"
EQUALS  = f"{termcolor.colored('=', 'blue')}"

binary = "../src/42sh"

def diff(expected: str, actual: str) -> str:
    expected_lines = expected.splitlines(keepends=True)
    actual_lines = actual.splitlines(keepends=True)
    return "".join(unified_diff(expected_lines, actual_lines, fromfile="expected", tofile="actual"))

def run_shell(*cmd_lines) -> subprocess.CompletedProcess:
    return subprocess.run(cmd_lines, capture_output=True, text=True)

#ref_proc = sp.run(["bash", "--posix"], capture_output=True, text=True)
#student_proc = sp.run([binary], capture_output=True, text=True)

def perform_checks(
    expected: subprocess.CompletedProcess, actual: subprocess.CompletedProcess
):
    assert (
        not expected.stderr
    ) or actual.stderr != "", "Something was expected on stderr"
    assert (
        expected.returncode == actual.returncode
    ), f"Exited with {actual.returncode} expected {expected.returncode}"
    assert (
        expected.stdout == actual.stdout
    ), f"stdout differ\n{diff(expected.stdout, actual.stdout)}"

@dataclass
class Category:
    tests: list[str]
    name: str

config = {"categories": ["execution/echo", "execution/for", "execution/if",
                         "execution/multiple_command",
                         "execution/not",
                         "syntax/bad",
                         "execution/variable",
                         "execution/operator",
                         "execution/piping"]}

if __name__ == "__main__":
    parser = ArgumentParser("TestSuite")
    parser.add_argument("--binary", required=True, type=Path)
    args = parser.parse_args()

    binary_path = args.binary.absolute()

    categories: list[Category] = []

    for category in config["categories"]:
        category = f"categories/{category}"
        categories.append(
                Category(
                    [
                        join(category, f)
                        for f in listdir(category)
                        if isfile(join(category, f))
                    ],
                    category,
                )
        )
    passed = 0
    failed = 0
    for category in categories:
        print(f"{EQUALS}" * 48)
        print(f"Category: {category.name}")
        for file in category.tests:
            print(file)
            sh_proc = run_shell(binary_path, file)
            sh_ref = run_shell("bash", "--posix", file)

            try:
                perform_checks(sh_ref, sh_proc)
            except AssertionError as err:
                print(f"{KO_TAG} {file}\n{err}")
                failed += 1
            else:
                print(f"{OK_TAG} {file}")
                passed += 1
                pass
    print(f"{EQUALS}" * 48)
    print(f"{termcolor.colored('passed ','green')} " + str(passed) + f" {termcolor.colored('failed ', 'red')}" + str(failed))

