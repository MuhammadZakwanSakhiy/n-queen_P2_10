
def quicksrot(arr):
    if len(arr) <= 1:
        return arr
    else:
        pivot = arr[0]
        less = [x for x in arr[1:] if x <= pivot]
        greater = [x for x in arr[1:] if x > pivot]

        print(f"Array   : {arr}")
        print(f"Pivot   : {pivot}")
        print(f"Less    : {less}")
        print(f"Greater : {greater}")
        print("")

        return quicksrot(less) + [pivot] + quicksrot(greater)


arr = [20, 7, 12, 9, 8, 14]

print("Sebelum:", arr)
print("Sesudah:", quicksrot(arr))
