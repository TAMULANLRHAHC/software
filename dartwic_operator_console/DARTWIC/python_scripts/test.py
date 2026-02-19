import dartwic as d

|test/test_1.buffer_size| = 3
|test/test_1.value| = 1
|test/test_1.value| = 2
|test/test_1.value| = 3
|test/test_1.value| = 1
|test/test_1.value| = 1
|test/test_1.value| = 1

average = |test/test_1.mean|

print(average)

test = d.promptYesNo(
            "RELAY ON?",
            "Is that jawn relay on.",
            ["jawn relay"],
            "check if that jawn is on or nahh."
        )

if (test == 1):
    print("YES")

if (test == 0):
    print("NO")

d.releaseHold("RUN TANK AUTO FILL HOLD")