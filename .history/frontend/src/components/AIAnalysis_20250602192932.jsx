import React, { useState } from 'react';
import axios from 'axios';

const AIAnalysis = () => {
  const [selectedFile, setSelectedFile] = useState(null);
  const [preview, setPreview] = useState(null);
  const [loading, setLoading] = useState(false);
  const [error, setError] = useState(null);

  const handleFileSelect = (event) => {
    const file = event.target.files[0];
    if (file) {
      setSelectedFile(file);
      const reader = new FileReader();
      reader.onloadend = () => {
        setPreview(reader.result);
      };
      reader.readAsDataURL(file);
    }
  };

  const handleUpload = async () => {
    if (!selectedFile) {
      setError('Please select an image first');
      return;
    }

    setLoading(true);
    setError(null);

    const formData = new FormData();
    formData.append('image', selectedFile);

    try {
      const response = await axios.post('http://127.0.0.1:8000/analyze-plant', formData, {
        headers: {
          'Content-Type': 'multipart/form-data',
        },
        responseType: 'blob'
      });

      // Create a blob from the PDF Stream
      const file = new Blob([response.data], { type: 'application/pdf' });
      // Create a URL for the blob
      const fileURL = window.URL.createObjectURL(file);
      // Open the PDF in a new window
      window.open(fileURL);
    } catch (err) {
      setError('Error analyzing image. Please try again.');
      console.error('Error:', err);
    } finally {
      setLoading(false);
    }
  };

  return (
    <div style={{ padding: '20px' }}>
      <h2 style={{ color: '#2e7d32', fontWeight: 700, fontSize: 22, marginBottom: 24, textAlign: 'center' }}>
        Plant Health Analysis
      </h2>

      <div style={{
        display: 'flex',
        flexDirection: 'column',
        alignItems: 'center',
        gap: '20px',
        maxWidth: '600px',
        margin: '0 auto',
        padding: '20px',
        backgroundColor: 'white',
        borderRadius: '12px',
        boxShadow: '0 2px 8px rgba(0,0,0,0.1)'
      }}>
        <div style={{
          width: '100%',
          height: '200px',
          border: '2px dashed #2e7d32',
          borderRadius: '8px',
          display: 'flex',
          flexDirection: 'column',
          alignItems: 'center',
          justifyContent: 'center',
          cursor: 'pointer',
          backgroundColor: preview ? 'transparent' : '#f5f8f5',
          position: 'relative',
          overflow: 'hidden'
        }}>
          {preview ? (
            <img
              src={preview}
              alt="Preview"
              style={{
                width: '100%',
                height: '100%',
                objectFit: 'cover'
              }}
            />
          ) : (
            <>
              <span style={{ fontSize: '48px', color: '#2e7d32' }}>📸</span>
              <p style={{ color: '#666', marginTop: '10px' }}>Click to upload plant image</p>
            </>
          )}
          <input
            type="file"
            accept="image/*"
            onChange={handleFileSelect}
            style={{
              position: 'absolute',
              width: '100%',
              height: '100%',
              opacity: 0,
              cursor: 'pointer'
            }}
          />
        </div>

        {error && (
          <div style={{
            color: '#d32f2f',
            backgroundColor: '#ffebee',
            padding: '10px',
            borderRadius: '4px',
            width: '100%',
            textAlign: 'center'
          }}>
            {error}
          </div>
        )}

        <button
          onClick={handleUpload}
          disabled={!selectedFile || loading}
          style={{
            backgroundColor: selectedFile && !loading ? '#2e7d32' : '#ccc',
            color: 'white',
            border: 'none',
            padding: '12px 24px',
            borderRadius: '6px',
            fontSize: '16px',
            fontWeight: '600',
            cursor: selectedFile && !loading ? 'pointer' : 'not-allowed',
            transition: 'background-color 0.3s',
            width: '100%'
          }}
          onMouseOver={(e) => {
            if (selectedFile && !loading) {
              e.currentTarget.style.backgroundColor = '#1b5e20';
            }
          }}
          onMouseOut={(e) => {
            if (selectedFile && !loading) {
              e.currentTarget.style.backgroundColor = '#2e7d32';
            }
          }}
        >
          {loading ? 'Analyzing...' : 'Analyze Plant Health'}
        </button>

        <div style={{ color: '#666', fontSize: '14px', textAlign: 'center' }}>
          Upload a clear image of your plant to get a detailed health analysis report
        </div>
      </div>
    </div>
  );
};

export default AIAnalysis; 