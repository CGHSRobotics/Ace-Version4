
import os


def main():

    path = "output.txt"

    # open file
    file = open(path, 'r')
    # read array
    line_arr = file.readlines()
    # close file
    file.close()

    # init start line vars
    start_line = -1
    end_line = -1

    # find start / end lines
    for line in line_arr:
        if "/start data" in line:
            start_line = line_arr.index(line)

        if "/end data" in line:
            end_line = line_arr.index(line)

    # if no start line, exit
    if start_line == -1:
        print("\n no start found\n")
        return
    # if no end line, exit
    if end_line == -1:
        print("\n no end found\n")
        return

    # get new file path
    new_path = input(
        "\n New File Name ('e' to exit)? \n    WARNING: THIS WILL OVERWRITE FILE OF SAME NAME! \n\n ")
    # if e, exit
    if new_path == "e":
        print("/n Exiting...")
        return

    # finish generating new path
    new_path = "l_data_logs/" + new_path + ".txt"
    # open new file based on path
    new_file = open(new_path, 'w+')

    # find full string to write
    stringToWrite = ""
    i = start_line + 1
    while i < end_line:
        stringToWrite += line_arr[i]
        i += 1

    # replace all single quotes with double quotes
    stringToWrite = stringToWrite.replace("'", '"')

    # format to json
    stringToWrite = '{ \n"array": [ \n {"msec": 0, "rpm": 0},' + \
        stringToWrite + '\n] }'

    # write to file
    new_file.write(stringToWrite)

    # close new file
    new_file.close()

    # Success!!
    print("\n File saved!\n")

    # ask if want to delete old output.txt file
    boolDelete = input(" Do you wish to delete 'output.txt'? (1 or 0)  ")
    # if true, overwrite output.txt
    if boolDelete == "1":
        print(" \nDeleting...\n")
        file = open(path, "w+")
        file.write("del")
        file.close()

    boolDelete = input("\n Do you wish to open chart.html? (1 or 0)  ")
    # if true, overwrite output.txt
    if boolDelete == "1":
        os.system("cmd 'start chart.html'")


main()
