from rest_framework import generics
from .models import Yolo
from .serializer import YoloSerializer

class YoloListCreate(generics.ListCreateAPIView):
    queryset = Yolo.objects.all()
    serializer_class = YoloSerializer

class YoloRetrieve(generics.RetrieveAPIView): 
    serializer_class = YoloSerializer