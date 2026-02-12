import subprocess
import matplotlib.pyplot as plt

# Same role as multiple scanf inputs in C
n_values = [1000, 2000, 3000]

# Same meaning as C variables
st_values = []   # Sequential Time (st)
pt_values = []   # Parallel Time (pt)

for n in n_values:
    # Execute compiled C program (./a.out)
    process = subprocess.Popen(
        ["./a.out"],
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        text=True
    )

    # Pass n to scanf("%d",&n)
    output, _ = process.communicate(str(n))

    # Extract st and pt exactly as printed in C
    for line in output.splitlines():

        if "Sequential Time" in line:
            st = float(line.split(":")[1].replace("sec", "").strip())
            st_values.append(st)

        if "Parallel Time" in line:
            pt = float(line.split(":")[1].replace("sec", "").strip())
            pt_values.append(pt)

# Plot Sequential Time (st)
plt.figure()
plt.plot(n_values, st_values, marker='o')
plt.xlabel("Matrix Size n")
plt.ylabel("Time (seconds)")
plt.title("Sequential Matrix Multiplication Time (st)")
plt.show()

# Plot Parallel Time (pt)
plt.figure()
plt.plot(n_values, pt_values, marker='o')
plt.xlabel("Matrix Size n")
plt.ylabel("Time (seconds)")
plt.title("Parallel Matrix Multiplication Time (pt)")
plt.show()
