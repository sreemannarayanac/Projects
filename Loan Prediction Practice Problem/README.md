# Loan Prediction Problem

## Project Details
The aim of this project is to predict if the person will be able to repay the loan or not. It is a binary classification problem.

## Data
The data is given in the form of a CSV file. It contains the following columns:
1. Loan_ID - Unique ID for each loan
2. Gender - Gender of the Loan Applicant
3. Married - Marital Status of the Loan Applicant
4. Dependents - Number of dependents of the Loan Applicant
5. Education - Education of the Loan Applicant
6. Self_Employed - Whether the Loan Applicant is self-employed or not
7. ApplicantIncome - Income of the Loan Applicant
8. CoapplicantIncome - Income of the Co-applicant
9. LoanAmount - Loan Amount in thousands
10. Loan_Amount_Term - Term of loan in months
11. Credit_History - Credit history meets guidelines
12. Property_Area - Area of property
13. Loan_Status - Loan approved (Y/N)

## Concepts Used
1. Hyper-parameter Tuning using GridSearchCV
2. Ensemble Learning (used Random Forest Classifier for the project)
3. Stratified K-folds
4. Feature Engineering
5. Missing Value Imputation
6. Outlier Treatment

## Models Used
1. Logistic Regression
2. Decision Tree
3. Random Forest Classifier

## Steps Performed
1. EDA Univariate Analysis
2. EDA Bivariate Analysis
3. Analysed relationship between Numerical Independent Variable and Target Variable.
4. Feature Engineering (derived new feature named Total Income using ApplicantIncome and Coapplicant income variables present in the data)
5. Missing Value imputation and Outlier Treatment
6. Model Building  
    a. Logistic Regression - with max accuracy = 0.796  
    b. Logistic Regression with Stratidfied K-folds (folds = 6)  - with max accuracy = .833  
    c. Decision Tree with Stratified K-folds (folds = 6) - with max accuracy = 0.766  
    d. Random Forest Classifier with Stratified K-fold (folds = 6) - with max accuracy = 0.864  
    e. GridSearchCV on Random Forest Classifier to find optimal hyper parameters.  