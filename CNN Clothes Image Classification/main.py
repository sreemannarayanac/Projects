import numpy as np
import streamlit as st
import tensorflow as tf
from tensorflow import keras
from tensorflow.keras.preprocessing.image import load_img
from tensorflow.keras.applications.xception import preprocess_input

st.set_page_config(page_title="Clothes Image Classification", page_icon="👚", layout="centered")
st.title("Clothes Image Classification")
st.write("Upload an image of a piece of clothing and we'll tell you what it is!")
st.write("This app uses a Convolutional Neural Network (CNN) to classify the image.")
st.write("The CNN was trained on https://github.com/alexeygrigorev/clothing-dataset-small dataset.")
st.write("The dataset contains of 10 classes: dress, hat, longsleeve, outwear, pants, shirt, shoes, shorts, skirt, t-shirt")

with st.spinner("Loading Model..."):
    model = keras.models.load_model("xception_v2_13_0.839.h5", compile=False)

# Toggle to take the input image from the user (camera or file upload)
camera_input = st.toggle("Use Camera for input?", False)

if camera_input:
    input = st.camera_input("Capture an image", )
else:
    input = st.file_uploader("Upload an image", type=["jpg", "jpeg", "png"])

if input:
    with st.spinner("Classifying..."):
        # Load the image
        img = load_img(input, target_size=(150, 150))
        x = np.array(img)
        X = np.array([x])
        X = preprocess_input(X)
        
        # Predict the class of the image
        predictions = model.predict(X)
        score = predictions[0] # These are the logits
        score_normalized = tf.nn.softmax(score).numpy() # These are the probabilities
        class_names = ['dress', 'hat', 'longsleeve', 'outwear', 'pants', 'shirt', 'shoes', 'shorts', 'skirt', 't-shirt']
        result = class_names[tf.argmax(score)]
        
        st.markdown(f"### Prediction: {result}", unsafe_allow_html=True)
        st.write(f"Probability of prediction: {score_normalized.max():.3f}")
        st.image(input, caption="Uploaded Image", use_column_width=True)