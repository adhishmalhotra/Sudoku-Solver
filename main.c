/*
Source used for getting the Sudoku solving algorithm:https://www.geeksforgeeks.org/sudoku-backtracking-7/
Functions that were taken from this source have a comment above them saying what exactly was used
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
// N is the size of the 2D matrix   N*N
#define N 9

//11 threads required for validation, therefore 11 values for checking the validity in the parent
int results[11] = {0,0,0,0,0,0,0,0,0,0,0};
int counter = 2;
int grid[N][N];

//creating the mutex
pthread_mutex_t mutex;


// Entire print function is from source listed
/* A utility function to print grid */
void print(int arr[N][N])
{
     for (int i = 0; i < N; i++)
      {
         for (int j = 0; j < N; j++)
            printf("%d ",arr[i][j]);
         printf("\n");
       }
}

// Entire isSafe function is from source listed
// Checks whether it will be legal
// to assign num to the
// given row, col
int isSafe(int grid[N][N], int row,
                       int col, int num)
{
     
    // Check if we find the same num
    // in the similar row , we return 0
    for (int x = 0; x <= 8; x++)
        if (grid[row][x] == num)
            return 0;
 
    // Check if we find the same num in the
    // similar column , we return 0
    for (int x = 0; x <= 8; x++)
        if (grid[x][col] == num)
            return 0;
 
    // Check if we find the same num in the
    // particular 3*3 matrix, we return 0
    int startRow = row - row % 3,
                 startCol = col - col % 3;
   
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            if (grid[i + startRow][j +
                          startCol] == num)
                return 0;
 
    return 1;
}

//Entire solveSudoku function is from source listed
/* Takes a partially filled-in grid and attempts
to assign values to all unassigned locations in
such a way to meet the requirements for
Sudoku solution (non-duplication across rows,
columns, and boxes) */
int solveSudoku(int grid[N][N], int row, int col)
{
     
    // Check if we have reached the 8th row
    // and 9th column (0
    // indexed matrix) , we are
    // returning true to avoid
    // further backtracking
    if (row == N - 1 && col == N)
        return 1;
 
    //  Check if column value  becomes 9 ,
    //  we move to next row and
    //  column start from 0
    if (col == N)
    {
        row++;
        col = 0;
    }
   
    // Check if the current position
    // of the grid already contains
    // value >0, we iterate for next column
    if (grid[row][col] > 0)
        return solveSudoku(grid, row, col + 1);
 
    for (int num = 1; num <= N; num++)
    {
         
        // Check if it is safe to place
        // the num (1-9)  in the
        // given row ,col  ->we move to next column
        if (isSafe(grid, row, col, num)==1)
        {
            /* assigning the num in the
               current (row,col)
               position of the grid
               and assuming our assigned num
               in the position
               is correct     */
            grid[row][col] = num;
           
            //  Checking for next possibility with next
            //  column
            if (solveSudoku(grid, row, col + 1)==1)
                return 1;
        }
       
        // Removing the assigned num ,
        // since our assumption
        // was wrong , and we go for next
        // assumption with
        // diff num value
        grid[row][col] = 0;
    }
    return 0;
}

void *checkCol(void *params)
{
    int (*array)[N][N] = params;
    int count = 0;
    //this is for iterating through the rows
    for(int i = 0; i < 9; i++){
       

        count = 0;

        //this is for iterating through the columns
        for(int j = 0; j < 9; j++){
            count += (*array)[j][i];
        }

        //checks the validity of the column
        if(count != 45){
            printf("Column: %i isn't valid!!, because the sum is %d\n", i, count);
            results[1] = 1;
        }else{
            continue;
        }

 
    }
}
 

void *checkRow(void *params){

    int (*array)[N][N] = params;
    int count = 0;
    //iterate through the rows
    for(int i = 0; i <= 8; i++){
       

        count = 0;

        //iterate through the elements within row
        for(int j = 0; j <= 8; j++){
            count += (*array)[i][j];
        }

        //checks the validity of the row
        if(count != 45){
            printf("row: %i isn't valid!!\n", i);
            results[0] = 1;
        }else{
            continue;
        }

 
    }

}
//reserving results[2] - results[10] for the checkbox function to report any errors
void *checkBox(void *params)
{
    int (*puzzle)[N][N] =  params;
    int row, col;
   
    for(row = 0; row<N; row+=3)
    {
        for(col = 0; col<N; col+=3)
        {
            int sum = 0;
            for(int p = row; p<3+row; p++)
                for(int q = col; q<3+col; q++)
                    sum += (*puzzle)[p][q];
            //printf("%d  ", sum);
            if(sum != 45)
            {
                pthread_mutex_lock(&mutex);
                results[counter] = 1;
                counter++;
                pthread_mutex_unlock(&mutex);
            }
           
            else
            {
                results[counter] = 0;
                counter++;
            }
           
        }
       
        //printf("\n");
    }
    pthread_exit(0);

}


int main(int argc, char* argv[])
{
    char *filename = argv[1];
    FILE *sudoku, *solution; 
    sudoku = fopen(filename, "r"); 
    
    for(int k = 0; k<9; k++)
    {
    
    	for(int l = 0; l<9; l++)
    	{
    	
    		fscanf(sudoku, "%d", &grid[k][l]);
    	
    	}	
    	
    }
    
 
   
    
    if (solveSudoku(grid, 0, 0)==1)
        print(grid);
    else
        printf("No solution exists");
    
    
    solution = fopen("solution.txt", "w");
    for(int k = 0; k<9; k++)
    {
    
    	for(int l = 0; l<9; l++)
    	{
    	
    		fprintf(solution, "%d ", grid[k][l]);
    	
    	}
    	
    	fprintf(solution, "\n");	
    	
    }
   
    // all lines above here in main function are from source listed

    //initializing 11 thread ids for creation
    pthread_t tid[11];

    //creating row check thread
    
    pthread_create(&tid[0],NULL,checkRow,grid);
    
    //joining row check thread
    
    pthread_join(tid[0], NULL);
    
    //creating the column check thread
    
    pthread_create(&tid[1],NULL,checkCol,grid);
    
    //joining the column check thread
   
   pthread_join(tid[1], NULL);
   
    //for checkbox
    for(int i = 2; i<=10; i++)
    {
        pthread_create(&tid[i],NULL,checkBox,grid);
    }

     for(int i = 2; i<=10; i++)
    {
        pthread_join(tid[i], NULL);
    }

    pthread_mutex_destroy(&mutex);
    //checks the results array to make sure all values are 0, if a value is 1 then a problem was detected by one of the worker threads
    
    int check = 1; 
    for(int j = 0; j < 11; j++){
    
    	
    	
        if(results[j] == 1){
            printf("thread worker: %d ran into an issue \nthe puzzle is not valid!\n",j); 
            check = 0; 
        }
        else
        {
            
            continue; 
        }
    }
    
    if (check)
    {
    
    printf("The solution within the selected text file can be solved and is saved to solution.txt\n "); 
    
    }
    

    fclose(sudoku); 
    fclose(solution); 
    return 0;
   
}

