import numpy as np
from keras.models import load_model
from PIL import Image

# Load the saved model
model = load_model('my_model.keras')

# Function to make predictions
def predict_with_model(img_path):
    img = Image.open(img_path)
    img = img.resize((224, 224))
    x = np.array(img) / 255.0  # Normalize the image data
    x = x.reshape(1, 224, 224, 3)  # Reshape the image for model input
    res = model.predict(x)
    classification = np.argmax(res)
    class_labels = {0: "It's a dog", 1: "It's a horse", 2: "It's a chicken", 3: "It's an elephant"}
    print(class_labels[classification])

# Example usage
img_path = r"./raw-img/elephant/1.jpg"  # Path to the image you want to classify
predict_with_model(img_path)
