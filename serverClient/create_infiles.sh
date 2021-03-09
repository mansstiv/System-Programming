#!/bin/bash

################################ functions ################################

# If not positive number exit 
# Being called for all numeric arguments
isPositiveNumber() 
{
    # First check if it's number
    if [[ $1 =~ ^[0-9]+$ ]]
    then
        # Check if positive
        if ! [ $1 -gt 0 ]
        then
            echo "Give positive numbers"
            exit 3
        fi
    else # negative or not a number
        echo "Give positive numbers"
        exit 3            
    fi                 
} 

################################ functions ################################

# checking if right number of parameters is given
if [ $# -ne 5 ]
then 
    echo "Invalid parameters"
    exit 1
fi    

# initialize variables with arguments given
diseasesFile=$1
countriesFile=$2
input_dir=$3
numFilesPerDirectory=$4
numRecordsPerFile=$5

# check numFilesPerDirectory and numRecordsPerFile are positive numbers
isPositiveNumber $numFilesPerDirectory
isPositiveNumber $numRecordsPerFile

# Initialize diseases, firstnames and lastnames arrays
IFS=$'\n' read -d '' -r -a diseases < $diseasesFile
declare -a firstnames=("Yannis" "Christofos" "Sofia" "Marianna" "Vagelis" "Maria" "Iosif" "Dionisis" "Konstantina"
                        "Theofilos" "Giorgos" "Dimitris");

declare -a lastnames=("Ioannidis" "Svingos" "Karvounari" "Rezkalla" "Nikolopoulos" "Berreta" "Koronis" "Gaitanis"
                        "Oikonomou" "Mailis" "Michas" "Halatsis");

# Array of patients
declare -a arrayOfRecords
# Characteristics of each patient
recordID=0
status=""
patientFirstName=""
patientLastName=""
disease=""
age=""
# Upper bound of for loop (total number of patients to be created)
max=$(($numFilesPerDirectory*$numRecordsPerFile))
# temp record
tempRecord=""
# useful variables for keeping indexes of recs
tempIndex=0
startIndex=0
tempVal=0
counter=0


# creating input directory
mkdir $input_dir

# reading countriesFile
while IFS= read -r line
do
    # creating countries folders
    mkdir $input_dir/$line

    counter=0
    startIndex=$recordID
    
    # For every country folder create temporary array of patients
    # Patients of array will be placed randomly to every date file 
    for ((a=0; a < $max; a++))
    do
        # Reinitialize recordID
        if [ "$a" -eq "$((3*$max/5))" ]
        then
            tempVal=$recordID
            recordID=$startIndex   
        fi
        # Initializing variables with random values
        patientFirstName=${firstnames[$((RANDOM % ${#firstnames[@]}))]}
        patientLastName=${lastnames[$((RANDOM % ${#lastnames[@]}))]}
        disease=${diseases[$((RANDOM % ${#diseases[@]}))]}
        age=$((1 + RANDOM % 120))
        # 3/4 of given id's have only entered
        # 1/4 of given id's have entered and exited
        if [ "$a" -lt "$((3*$max/5))" ]
        then      
            arrayOfRecords[$a]="$recordID ENTER $patientFirstName $patientLastName $disease $age"
        else
            tempRecord=${arrayOfRecords[$(($counter))]}
            tempRecord=${tempRecord//ENTER/EXIT} # replace ENTER with EXIT 
            arrayOfRecords[$a]=$tempRecord
            counter=$(($counter+1))
        fi
        recordID=$(($recordID+1)) 
    done

    # reinitialize recordID
    recordID=$tempVal

    for ((a=0; a < $numFilesPerDirectory; a++))
    do
        b=1
        # creating date files for each country
        while [ $b -eq 1 ] 
        do
            tempYear=$((RANDOM % 10)) # choosing year (2010 + tempYear)
            tempDate=$((1 + RANDOM % 365)) # choosing date
            result=$(($tempYear*365+$tempDate))
            date=$(date -d "2010-01-01 + $result days" +'%d-%m-%Y')
            # If date already exist try again
            if [ -f "$input_dir/$line/$date" ] 
            then
                continue
            else
                touch $input_dir/$line/$date
                # writing records to each date file
                for ((c=0; c < $numRecordsPerFile; c++))
                do
                    tempIndex=$((RANDOM % ${#arrayOfRecords[@]}))
                    tempRecord=${arrayOfRecords[$tempIndex]}
                    echo "$tempRecord" >> "$input_dir/$line/$date"
                    unset arrayOfRecords[$tempIndex]
                    # reinitialize indexes of array
                    arrayOfRecords=("${arrayOfRecords[@]}")
                done                
                break 
            fi
        done        
    done
done < "$countriesFile"