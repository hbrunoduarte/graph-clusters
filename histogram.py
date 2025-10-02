import pandas as pd
import matplotlib.pyplot as plt
from matplotlib.ticker import MultipleLocator

def plot_components(filename, title):
    df = pd.read_csv(filename)

    df.columns = df.columns.str.strip()
    df["size"] = pd.to_numeric(df["size"], errors="coerce")

    counts = df["size"].value_counts().sort_index()

    plt.figure(figsize=(8,5))
    plt.bar(counts.index, counts.values, width=0.8, edgecolor="black")

    plt.xlabel("Connected component size")
    plt.ylabel("Amount of components")
    plt.title(title)
    plt.grid(axis="y", linestyle="--", alpha=0.7)

    plt.gca().xaxis.set_major_locator(MultipleLocator(1))
    plt.gca().yaxis.set_major_locator(MultipleLocator(1))

    plt.show()


def main():
    while True:
        print("\n=== Select a file to visualize ===")
        print("1 - components1.csv (threshold = 0.0)")
        print("2 - components2.csv (threshold = 0.3)")
        print("3 - components3.csv (threshold = 0.5)")
        print("4 - components4.csv (threshold = 0.9)")
        print("0 - Exit")

        choice = input("Type your option: ")

        if choice == "1":
            plot_components("tarefa1/clusters/components1.csv", "Components distribution (Threshold = 0.0)")
        elif choice == "2":
            plot_components("tarefa1/clusters/components2.csv", "Components distribution (Threshold = 0.3)")
        elif choice == "3":
            plot_components("tarefa1/clusters/components3.csv", "Components distribution (Threshold = 0.5)")
        elif choice == "4":
            plot_components("tarefa1/clusters/components4.csv", "Components distribution (Threshold = 0.9)")
        elif choice == "0":
            print("Leaving...")
            break
        else:
            print("Invalid option! Try again.")

if __name__ == "__main__":
    main()
