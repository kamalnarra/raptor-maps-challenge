# raptor-maps-challenge

Here is my code for the tech screen. I had a lot of fun implementing it and hope you enjoy my solution :blush:

The project description makes it clear that this challenge was designed to simulate a real world situation, so I implemented my solution accordingly. 

Instead of independently calculating the distances, I used a Pandas dataframe to represent all of the data given to me alongside an adjancy matrix of every technician's proximity to one another. Additionally, because querying/manipulating time-series data in a standard dataframe is inefficient at scale, I multi-indexed the data by datetime, id, and technician name. 

I did some exploratory data analysis for Part 2 of the the challenge. First I graphed the movement of each technician over time to see if there were any easily discernable patterns. Since knowing in which direction a technician would move is crucial in predicting their next position, I also created a scatterplot displaying the relationship between a technician's bearing and their angle of movement. There wasn't a clear linear relationship, but I saw a few interesting cluster's that I would like to explore with more data. A potential solution that I'd like to test out would be using a Markov model to predict the final destination based on the both the bearing and some data on the damage risk levels for the panels the technicians are operating on.

Here is my code for the tech screen. I had a lot of fun implementing it and hope you enjoy my solution blush

### Part One

The project description makes it clear that this challenge was designed to simulate a real0world situation, so I implemented my solution accordingly.

Instead of independently calculating the distances, I used a Pandas dataframe to represent all of the data given to me alongside an adjancy matrix of every technician's proximity to one another. 

Additionally, because querying/manipulating time-series data in a standard dataframe is inefficient at scale, I multi-indexed the data by datetime, id, and technician name. 

I also ensured that the dataframe and its corresponding functions work for any number of technicians per datetime, making the code reusable as more data is ingested.

### Part Two
I also did some exploratory data analysis to determine which approach to take. First, I graphed the movement of each technician over time to see if there were any easily discernable patterns. 

Since knowing in which direction a technician will move is crucial in predicting their next position, I also created a scatterplot displaying the relationship between a technician's bearing and their angle of movement. There wasn't a clear linear relationship, but I saw a few intriguing clusters I'd like to explore with more data. 

A potential solution I would like to test out would be using a Markov model to predict the final destination based on the both the bearing and some additional data on the likelihood of damage for different geographic areas.
