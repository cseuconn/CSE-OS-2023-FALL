import requests
import time

def test_get_request():
    response = requests.get("http://127.0.0.1:8001")
    print("GET Request Response:", response.json())

def test_post_request():
    item = {
        "name": "Sample Item",
        "description": "This is a sample item",
        "price": 10.5,
        "tax": 1.5
    }
    response = requests.post("http://127.0.0.1:8001/items/", json=item)
    print("POST Request Response:", response.json())

if __name__ == "__main__":
    # test_get_request()
    test_post_request()

