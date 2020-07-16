// Name: Hamzah Ismail
// ID: 20263213
// Ver: 1.0.0
// Date: 30/12/15
// Desc: SchoolTripsCalculator.cpp

// Including additional libraries for formatting
#include <iostream>
#include <iomanip>

using namespace std;

int main()
{
    // Declaring and assigning all variables
    const double BREAKFAST = 1.50;
    const double LUNCH = 2.00;
    const double DINNER = 3.50;
    int num_students;
    int num_adults;
    double vehicle_cost;
    int trip_length;
    double trip_distance;
    double miles;
    double fuel_cost;
    double food_cost;
    int dietary;
    int non_dietary;
    double dietary_cost;
    double non_dietary_cost;
    double staff_food_cost;
    double total_trip_cost;

    // Declared the variable for the loop
    int i = 10;

    //Using a loop to ensure the user can run the program multiple times
    do{
        // Output message assisting the user
        cout << "Please input the number of students participating on this trip (If you wish to end the program, enter 0 or below): ";
        cin >> num_students;

        //If statements for simplicity and error checking, with appropriate output statements
        if (num_students <= 0)
            {
                cout << "Terminating program."; break;
            }
        else if (num_students >= 1 && num_students <= 12)
            {
                num_adults = 1;
                cout << "The number of adults required for this trip is: " << num_adults << endl;
            }
        else if (num_students >= 13 && num_students <= 24)
            {
                num_adults = 2;
                cout << "The number of adults required for this trip is: " << num_adults << endl;
            }
        else if (num_students >= 25 && num_students <= 36)
            {
                num_adults = 3;
                cout << "The number of adults required for this trip is: " << num_adults << endl;
            }
        else if (num_students >= 37 && num_students <= 48)
            {
                num_adults = 4;
                cout << "The number of adults required for this trip is: " << num_adults << endl;
            }
        else if (num_students >= 49 && num_students <= 60)
            {
                num_adults = 5;
                cout << "The number of adults required for this trip is: " << num_adults << endl;
            }
        else if (num_students >= 61 && num_students <= 72)
            {
                num_adults = 6;
                cout << "The number of adults required for this trip is: " << num_adults << endl;
            }
        else if (num_students >= 73 && num_students <= 84)
            {
                num_adults = 7;
                cout << "The number of adults required for this trip is: " << num_adults << endl;
            }
        else if (num_students >= 85 && num_students <= 96)
            {
                num_adults = 8;
                cout << "The number of adults required for this trip is: " << num_adults << endl;
            }
        else if (num_students >= 97 && num_students <= 108)
            {
                num_adults = 9;
                cout << "The number of adults required for this trip is: " << num_adults << endl;
            }
        else if (num_students >= 109 && num_students <= 120)
            {
                num_adults = 10;
                cout << "The number of adults required for this trip is: " << num_adults << endl;
            }
        else
            {
                cout << "The trip is too large. School understaffed. " << endl;
            }

        // Using previous data throughout the program, proving re-usability of code in the following if statements. Again, used for simplicity and error checking with appropriate outputs
        // Manipulated data through the use of the additional library to provide the user with clear data
        if (num_students <= 0 || num_students > 120)
            {
                cout << "Error"; break;
            }
        else if (num_students >= 1 && num_students <= 12)
            {
                vehicle_cost = 40.00;
                cout << "\nThe vehicle required for this trip is 1 minibus, costing: " << setiosflags(ios::fixed) << setprecision(2) << vehicle_cost << " GBP" << endl;
            }
        else if (num_students >= 13 && num_students <= 24)
            {
                vehicle_cost = 60.00;
                cout << "\nThe vehicle required for this trip is 1 coach, costing: " << setiosflags(ios::fixed) << setprecision(2) << vehicle_cost << " GBP" << endl;
            }
        else if (num_students >= 25 && num_students <= 36)
            {
                vehicle_cost = 60.00;
                cout << "\nThe vehicle required for this trip is 1 coach, costing: " << setiosflags(ios::fixed) << setprecision(2) << vehicle_cost << " GBP" << endl;
            }
        else if (num_students >= 37 && num_students <= 48)
            {
                vehicle_cost = 100.00;
                cout << "\nThe vehicles required for this trip are 1 minibus and 1 coach, costing: " << setiosflags(ios::fixed) << setprecision(2) << vehicle_cost << " GBP" << endl;
            }
        else if (num_students >= 49 && num_students <= 60)
            {
                vehicle_cost = 120.00;
                cout << "\nThe vehicles required for this trip are 2 coaches, costing: " << setiosflags(ios::fixed) << setprecision(2) << vehicle_cost << " GBP" << endl;
            }
        else if (num_students >= 61 && num_students <= 72)
            {
                vehicle_cost = 120.00;
                cout << "\nThe vehicles required for this trip are 2 coaches, costing: " << setiosflags(ios::fixed) << setprecision(2) << vehicle_cost << " GBP" << endl;
            }
        else if (num_students >= 73 && num_students <= 84)
            {
                vehicle_cost = 160.00;
                cout << "\nThe vehicles required for this trip are 1 minibus and 2 coaches, costing: " << setiosflags(ios::fixed) << setprecision(2) << vehicle_cost << " GBP" << endl;
            }
        else if (num_students >= 85 && num_students <= 96)
            {
                vehicle_cost = 180.00;
                cout << "\nThe vehicles required for this trip are 3 coaches, costing: " << setiosflags(ios::fixed) << setprecision(2) << vehicle_cost << " GBP" << endl;
            }
        else if (num_students >= 97 && num_students <= 108)
            {
                vehicle_cost = 180.00;
                cout << "\nThe vehicles required for this trip are 3 coaches, costing: " << setiosflags(ios::fixed) << setprecision(2) << vehicle_cost << " GBP" << endl;
            }
        else
            {
                vehicle_cost = 220.00;
                cout << "\nThe vehicles required for this trip are 1 minibus and 3 coaches, costing: " << setiosflags(ios::fixed) << setprecision(2) << vehicle_cost << " GBP" << endl;
            }

        // Output message assisting the user
        cout << "\nPlease enter the length of the trip. Enter 1 if the trip is a short day trip, 2 if the trip is a long day trip and 3 if the trip is an overnight trip: ";
        cin >> trip_length;

        //If statements for simplicity and error checking, with appropriate output statements
        if (trip_length <= 0 || trip_length >= 4 || num_students <= 0 || num_students > 120)
            {
                cout << "Error"; break;
            }
        else if (trip_length == 1)
            {
                cout << "\nThe length of this trip is a short day trip." << endl;
            }
        else if (trip_length == 2)
            {
                cout << "\nThe length of this trip is a long day trip." << endl;
            }
        else if (trip_length == 3)
            {
                cout << "\nThe length of this trip is an overnight trip." << endl;
            }

        // Output message assisting the user
        cout << "\nPlease enter the number of students requiring dietary meals: ";
        cin >> dietary;

        // Error check implemented with an if statement
        if (dietary > num_students)
            {
                cout << "Error."; break;
            }

        // Mathematical formula
        non_dietary = num_students - dietary;

        //If statements for simplicity and error checking, with appropriate output statements
        if (trip_length <= 0 || trip_length >= 4 || num_students <= 0 || num_students > 120)
            {
                cout << "Error"; break;
            }
        else if (trip_length == 1)
            {
                // Mathematical formulas
                non_dietary_cost = non_dietary * LUNCH;
                dietary_cost = dietary * (LUNCH * 1.08);
                staff_food_cost = num_adults * (LUNCH * 1.20);
                food_cost = non_dietary_cost + dietary_cost + staff_food_cost;

                // Output messages assisting the user
                // Manipulated data for user clarity
                cout << "\nThe cost for the non-dietary students will be: " << setiosflags(ios::fixed) << setprecision(2) << non_dietary_cost << " GBP";
                cout << "\nThe cost for the dietary students will be: " << setiosflags(ios::fixed) << setprecision(2) << dietary_cost << " GBP";
                cout << "\nThe cost for the staff will be: " << setiosflags(ios::fixed) << setprecision(2) << staff_food_cost << " GBP";
                cout << "\nThe total food cost for this trip will be: " << setiosflags(ios::fixed) << setprecision(2) << food_cost << " GBP." << endl;
            }
        else if (trip_length == 2)
            {
                // Mathematical formulas
                non_dietary_cost = non_dietary * (LUNCH + DINNER);
                dietary_cost = dietary * ((LUNCH + DINNER) * 1.08);
                staff_food_cost = num_adults * ((LUNCH + DINNER) * 1.20);
                food_cost = non_dietary_cost + dietary_cost + staff_food_cost;

                // Output messages assisting the user
                // Manipulated data for user clarity
                cout << "\nThe cost for the non-dietary students will be: " << setiosflags(ios::fixed) << setprecision(2) << non_dietary_cost << " GBP";
                cout << "\nThe cost for the dietary students will be: " << setiosflags(ios::fixed) << setprecision(2) << dietary_cost << " GBP";
                cout << "\nThe cost for the staff will be: " << setiosflags(ios::fixed) << setprecision(2) << staff_food_cost << " GBP";
                cout << "\nThe total food cost for this trip will be: " << setiosflags(ios::fixed) << setprecision(2) << food_cost << " GBP." << endl;
            }
        else if (trip_length == 3)
            {
                // Mathematical formulas
                non_dietary_cost = non_dietary * (LUNCH + DINNER + BREAKFAST);
                dietary_cost = dietary * ((LUNCH + DINNER + BREAKFAST) * 1.08);
                staff_food_cost = num_adults * ((LUNCH + DINNER + BREAKFAST) * 1.20);
                food_cost = non_dietary_cost + dietary_cost + staff_food_cost;

                // Output messages assisting the user
                // Manipulated data for user clarity
                cout << "\nThe cost for the non-dietary students will be: " << setiosflags(ios::fixed) << setprecision(2) << non_dietary_cost << " GBP";
                cout << "\nThe cost for the dietary students will be: " << setiosflags(ios::fixed) << setprecision(2) << dietary_cost << " GBP";
                cout << "\nThe cost for the staff will be: " << setiosflags(ios::fixed) << setprecision(2) << staff_food_cost << " GBP";
                cout << "\nThe total food cost for this trip will be: " << setiosflags(ios::fixed) << setprecision(2) << food_cost << " GBP." << endl;
            }

        // Output message assisting the user
        cout << "\nPlease enter the distance to the destination in miles: ";
        cin >> miles;
        if (miles <= 0)
            {
                cout << "Error. Invalid input"; break;
            }

        // Mathematical formulas
        trip_distance = miles * 2;
        fuel_cost = trip_distance * 0.40;

        // Output message assisting the user
        cout << "\nThe total distance to be travelled, both ways, is: " << trip_distance << " miles, costing: ";
        cout << setiosflags(ios::fixed) << setprecision(2) << fuel_cost << " GBP" << endl;

        total_trip_cost = vehicle_cost + food_cost + fuel_cost;

        // Output message assisting the user
        cout << "\nThe costs for this trip are as follows: \n" << "\nNumber of students: " << num_students << "\nNumber of staff: " << num_adults;

        //If statements for simplicity and error checking, with appropriate output statements
        if (vehicle_cost == 40)
            {
                cout << "\nVehicle required: 1 Minibus" << endl;
            }
        else if (vehicle_cost == 60)
            {
                cout << "\nVehicle required: 1 Coach" << endl;
            }
        else if (vehicle_cost == 100)
            {
                cout << "\nVehicles required: 1 Minibus & 1 Coach" << endl;
            }
        else if (vehicle_cost == 120)
            {
                cout << "\nVehicles required: 2 Coaches" << endl;
            }
        else if (vehicle_cost == 160)
            {
                cout << "\nVehicles required: 1 Minibus & 2 Coaches" << endl;
            }
        else if (vehicle_cost == 180)
            {
                cout << "\nVehicles required: 3 Coaches" << endl;
            }
        else if (vehicle_cost == 220)
            {
                cout << "\nVehicles required: 1 Minibus & 3 Coaches" << endl;
            }

        // Output message assisting the user
        cout << "Vehicle cost: " << setiosflags(ios::fixed) << setprecision(2) << vehicle_cost << " GBP" << "\nTrip length: ";

        //If statements for simplicity and error checking, with appropriate output statements
        if (trip_length == 1)
            {
                cout << "Short day" << "\n";
            }
        else if (trip_length == 2)
            {
                cout << "Long day" << "\n";
            }
        else if (trip_length == 3)
            {
                cout << "Overnight" << "\n";
            }

        // Output messages assisting the user
        // Manipulated data for user clarity
        cout << "\nThe food costs for this trip are: \n" << "\nNon-dietary: " << setiosflags(ios::fixed) << setprecision(2) << non_dietary_cost << " GBP \nDietary: ";
        cout << setiosflags(ios::fixed) << setprecision(2) << dietary_cost << " GBP \nStaff cost: " << setiosflags(ios::fixed) << setprecision(2) << staff_food_cost << " GBP \n";
        cout << "\nDistance: " << trip_distance << " mi \nFuel cost: " << setiosflags(ios::fixed) << setprecision(2) << fuel_cost << " GBP \n" << "\n==========" << "\nTotal cost: ";
        cout << setiosflags(ios::fixed) << setprecision(2) << total_trip_cost << " GBP" << "\n==========" << "\n" << endl;
    } while(i > 2); // Loop parameters

    return 0;
}
