from rest_framework import serializers
from .models import Yolo

class YoloSerializer(serializers.ModelSerializer):
    class Meta:
        model = Yolo
        fields = '__all__'
