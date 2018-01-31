
-- --------------------------------------------------
-- Entity Designer DDL Script for SQL Server 2005, 2008, 2012 and Azure
-- --------------------------------------------------
-- Date Created: 01/31/2018 21:08:20
-- Generated from EDMX file: D:\Github\WeatherStationv2\VS\LocalServer\WeatherService\StorageModel.edmx
-- --------------------------------------------------

SET QUOTED_IDENTIFIER OFF;
GO
USE [WeatherDB];
GO
IF SCHEMA_ID(N'dbo') IS NULL EXECUTE(N'CREATE SCHEMA [dbo]');
GO

-- --------------------------------------------------
-- Dropping existing FOREIGN KEY constraints
-- --------------------------------------------------


-- --------------------------------------------------
-- Dropping existing tables
-- --------------------------------------------------


-- --------------------------------------------------
-- Creating all tables
-- --------------------------------------------------

-- Creating table 'MeasurementSet'
CREATE TABLE [dbo].[MeasurementSet] (
    [MeasurementId] int IDENTITY(1,1) NOT NULL,
    [OutsideTemperature] decimal(18,0)  NULL,
    [OutsideHumidity] decimal(18,0)  NULL,
    [Pressure] decimal(18,0)  NULL,
    [WindSpeed] decimal(18,0)  NULL,
    [WindDir] smallint  NULL,
    [GustSpeed] decimal(18,0)  NULL,
    [GustDir] smallint  NULL,
    [DewPoint] decimal(18,0)  NULL,
    [TimeStamp] date  NOT NULL
);
GO

-- Creating table 'RainMeasurementSet'
CREATE TABLE [dbo].[RainMeasurementSet] (
    [TimeStamp] time  NOT NULL,
    [Rain] decimal(18,0)  NOT NULL,
    [Measurement_MeasurementId] int  NOT NULL
);
GO

-- --------------------------------------------------
-- Creating all PRIMARY KEY constraints
-- --------------------------------------------------

-- Creating primary key on [MeasurementId] in table 'MeasurementSet'
ALTER TABLE [dbo].[MeasurementSet]
ADD CONSTRAINT [PK_MeasurementSet]
    PRIMARY KEY CLUSTERED ([MeasurementId] ASC);
GO

-- Creating primary key on [TimeStamp] in table 'RainMeasurementSet'
ALTER TABLE [dbo].[RainMeasurementSet]
ADD CONSTRAINT [PK_RainMeasurementSet]
    PRIMARY KEY CLUSTERED ([TimeStamp] ASC);
GO

-- --------------------------------------------------
-- Creating all FOREIGN KEY constraints
-- --------------------------------------------------

-- Creating foreign key on [Measurement_MeasurementId] in table 'RainMeasurementSet'
ALTER TABLE [dbo].[RainMeasurementSet]
ADD CONSTRAINT [FK_MeasurementRainMeasurement]
    FOREIGN KEY ([Measurement_MeasurementId])
    REFERENCES [dbo].[MeasurementSet]
        ([MeasurementId])
    ON DELETE NO ACTION ON UPDATE NO ACTION;
GO

-- Creating non-clustered index for FOREIGN KEY 'FK_MeasurementRainMeasurement'
CREATE INDEX [IX_FK_MeasurementRainMeasurement]
ON [dbo].[RainMeasurementSet]
    ([Measurement_MeasurementId]);
GO

-- --------------------------------------------------
-- Script has ended
-- --------------------------------------------------