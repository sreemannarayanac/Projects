FROM python:3.9-slim

COPY requirements.txt .
COPY xception_v2_13_0.839.h5 .
COPY main.py .

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    python3-dev \
    && rm -rf /var/lib/apt/lists/*

RUN pip install --upgrade pip

RUN pip install -r requirements.txt

# Expose the port for Streamlit
EXPOSE 8501

CMD ["streamlit", "run", "main.py"]