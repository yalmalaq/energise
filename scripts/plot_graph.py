import plotly.express as px 
import pandas as pd 
import plotly.graph_objs as go

data = pd.read_csv('data.csv')

fig = px.bar(data,x='Time', y='Active', color='Cores',height=400, width=700)
fig.write_image('graph.png')