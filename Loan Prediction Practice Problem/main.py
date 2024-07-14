import streamlit as st
import mlflow
import pandas as pd
import numpy as np
from sklearn.impute import SimpleImputer

# Configuring the page
st.set_page_config(
    "Loan Prediction Problem",
    page_icon=":moneybag:",
    menu_items={"Get Help": "https://www.linkedin.com/in/sreemannarayana-chamarti/", "About": "This is a simple web app to predict whether a person is eligible for a loan or not."}
)

st.title("Loan Prediction Problem")
st.subheader("The data taken into consideration is from the Loan Prediction Practice Problem from Analytics Vidhya.")
st.markdown('''
        The data consists of the following columns:
        - Loan_ID
        - Gender
        - Married
        - Dependents
        - Education
        - Self_Employed
        - ApplicantIncome
        - CoapplicantIncome
        - LoanAmount
        - Loan_Amount_Term
        - Credit_History
        - Property_Area
        - Loan_Status
        ''', unsafe_allow_html=True)

# Input features
loan_id = "LP001015"

# Divide the page into two columns
col1, col2 = st.columns(2)

with col1:
    gender = st.selectbox("Select your gender", ["Male", "Female"], placeholder="Select", help="Your gender: Male or Female")
    married = st.selectbox("Are you married?", ["Yes", "No"], placeholder="Select", help="Your marital status: Yes if married, No if single")
    dependents = st.selectbox("Number of dependents", ["0", "1", "2", "3+"], placeholder="Select", help="Number of people dependent on you: 0, 1, 2, or 3+ for 3 or more")
    education = st.selectbox("Education", ["Graduate", "Not Graduate"], placeholder="Select", help="Your education level: Graduate for a bachelor's degree or higher, Not Graduate otherwise")
    employed = st.selectbox("Are you self-employed?", ["Yes", "No"], placeholder="Select", help="Your employment status: Yes if self-employed, No if employed by others")
    applicant_income = st.number_input("Applicant Income", min_value=0, max_value=100000, value=0, step=1000, help="Your annual income in thousands (e.g., 50 for 50,000)")

with col2:
    coapplicant_income = st.number_input("Coapplicant Income", min_value=0, max_value=100000, value=0, step=1000, help="Annual income of the co-applicant (if any) in thousands")
    loan_amount = st.number_input("Loan Amount", min_value=0, max_value=100000, value=0, step=1000, help="The loan amount you're requesting in thousands")
    loan_amount_term = st.number_input("Loan Amount Term", min_value=0, max_value=1000, value=0, step=12, help="The loan term in months (e.g., 360 for 30 years)")
    credit_history = st.selectbox("Credit History", ["0", "1"], placeholder="Select", help="Your credit history: 1 if you have a history of paying debts, 0 if not")
    property_area = st.selectbox("Property Area", ["Urban", "Semiurban", "Rural"], placeholder="Select", help="The location of the property: Urban, Semiurban, or Rural")

# Predict button
predict = st.button("Predict")

# Make the data into a pandas DataFrame
if predict:
    data = pd.DataFrame({
        "Credit_History": float(credit_history),  # Convert to float
        "Loan_ID": loan_id,
        "Gender": gender,
        "Married": married,
        "Dependents": dependents,
        "Education": education,
        "Self_Employed": employed,
        "ApplicantIncome": applicant_income,
        "CoapplicantIncome": coapplicant_income,
        "LoanAmount": loan_amount,
        "Loan_Amount_Term": loan_amount_term,
        "Property_Area": property_area,
    }, index=[0])

    # Preprocess the data
    data["LoanAmount_log"] = np.log(data["LoanAmount"] + 1)
    data["Total_Income"] = data["ApplicantIncome"] + data["CoapplicantIncome"]
    data["Total_Income_log"] = np.log(data["Total_Income"] + 1)
    data["EMI"] = data["LoanAmount"] / data["Loan_Amount_Term"]
    data["Balance Income"] = data["Total_Income"] - (data["EMI"] * 1000)

    # Add dummy variables
    data = pd.get_dummies(data)
    
    # Ensure all necessary columns are present
    required_columns = [
        'Credit_History', 'LoanAmount_log', 'Gender_Female', 'Gender_Male',
        'Married_No', 'Married_Yes', 'Dependents_0', 'Dependents_1',
        'Dependents_2', 'Dependents_3+', 'Education_Graduate',
        'Education_Not Graduate', 'Self_Employed_No', 'Self_Employed_Yes',
        'Property_Area_Rural', 'Property_Area_Semiurban', 'Property_Area_Urban',
        'Total_Income', 'Total_Income_log', 'EMI', 'Balance Income'
    ]
    for col in required_columns:
        if col not in data.columns:
            data[col] = 0

    # Convert to the correct types
    data['Credit_History'] = data['Credit_History'].astype(float)
    data['LoanAmount_log'] = data['LoanAmount_log'].astype(float)
    data['Total_Income'] = data['Total_Income'].astype(float)
    data['Total_Income_log'] = data['Total_Income_log'].astype(float)
    data['EMI'] = data['EMI'].astype(float)
    data['Balance Income'] = data['Balance Income'].astype(float)

    data['Gender_Female'] = data['Gender_Female'].astype(bool)
    data['Gender_Male'] = data['Gender_Male'].astype(bool)
    data['Married_No'] = data['Married_No'].astype(bool)
    data['Married_Yes'] = data['Married_Yes'].astype(bool)
    data['Dependents_0'] = data['Dependents_0'].astype(bool)
    data['Dependents_1'] = data['Dependents_1'].astype(bool)
    data['Dependents_2'] = data['Dependents_2'].astype(bool)
    data['Dependents_3+'] = data['Dependents_3+'].astype(bool)
    data['Education_Graduate'] = data['Education_Graduate'].astype(bool)
    data['Education_Not Graduate'] = data['Education_Not Graduate'].astype(bool)
    data['Self_Employed_No'] = data['Self_Employed_No'].astype(bool)
    data['Self_Employed_Yes'] = data['Self_Employed_Yes'].astype(bool)
    data['Property_Area_Rural'] = data['Property_Area_Rural'].astype(bool)
    data['Property_Area_Semiurban'] = data['Property_Area_Semiurban'].astype(bool)
    data['Property_Area_Urban'] = data['Property_Area_Urban'].astype(bool)

    # Impute missing values
    imputer = SimpleImputer(strategy="mean")
    data[required_columns] = imputer.fit_transform(data[required_columns])

    new_data = pd.DataFrame(data, columns=required_columns)
    
    # Convert float columns to boolean for relevant columns
    boolean_columns = [
        'Gender_Female', 'Gender_Male', 'Married_No', 'Married_Yes',
        'Dependents_0', 'Dependents_1', 'Dependents_2', 'Dependents_3+',
        'Education_Graduate', 'Education_Not Graduate',
        'Self_Employed_No', 'Self_Employed_Yes',
        'Property_Area_Rural', 'Property_Area_Semiurban', 'Property_Area_Urban'
    ]
    
    for col in boolean_columns:
        new_data[col] = new_data[col].astype(float) >= 0.5

    with st.spinner("Predicting..."):
        # Load the model
        logged_model = 'mlruns/0/5892f2480dfc4103ad53071daf5c0f2c/artifacts/best_estimator/'

        # Load model as a PyFuncModel.
        loaded_model = mlflow.pyfunc.load_model(logged_model)

        # Predict on a Pandas DataFrame.
        prediction = loaded_model.predict(new_data)

        # Display the prediction in a human-readable format
        if prediction[0] == 1:
            st.success("Congratulations! You are eligible for a loan.")
            st.balloons()
        else:
            st.error("Sorry, you are not eligible for a loan.")
            st.info("Possible reasons for rejection: low income, high loan amount, or poor credit history.")